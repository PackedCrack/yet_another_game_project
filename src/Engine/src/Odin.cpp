//
// Created by qwerty on 11/08/2025.
//
#include "Odin.hpp"

#include "Entity.hpp"
#include "FilepathResolver.hpp"
#include "Input.hpp"
#include "components/Children.hpp"
#include "components/DirtyFlag.hpp"
#include "components/LocalTRS.hpp"
#include "components/Geometry.hpp"
#include "components/Model.hpp"
#include "components/Parent.hpp"
#include "components/WorldTRS.hpp"
#include "graphics/Graphics.hpp"
#include "graphics/gpu_types.hpp"
#include "state/Input.hpp"
#include "window/Window.hpp"
// Debug
#include <debug/Logger.hpp>
#include <debug/debug_defines.hpp>
// AssetLoader
#include <assetloader/AssetRegistry.hpp>
#include <assetloader/SceneGraph.hpp>
//
//
namespace
{
using namespace odin;
//
//
inline static void compose(const component::WorldTRS& pw, const component::LocalTRS& loc, component::WorldTRS& out)
{
    out.orientation = glm::normalize(pw.orientation * loc.orientation);
    out.scale = pw.scale * loc.scale;
    //out.translation = pw.translation + glm::rotate(pw.orientation, pw.scale * loc.translation);
    out.translation = pw.translation + (pw.orientation * (pw.scale * loc.translation));
}
// Recompute subtree starting at 'top'.
// 'parent_changed' must be true for 'top' if its parent's world changed, or if 'top' itself was dirty.
static void recompute_subtree(details::ECS& ecs, Entity top, bool parent_changed)
{
    // Non-recursive DFS
    struct Node
    {
        Entity e;
        bool parent_changed;
    };
    std::vector<Node> stack;
    stack.push_back({ top, parent_changed });

    // Track visited to avoid double work if subtrees overlap in the input set
    std::unordered_set<entt::entity> visited;

    while (!stack.empty())
    {
        Node n = stack.back();
        stack.pop_back();

        entt::entity raw = n.e.to_underlying();
        if (!visited.insert(raw).second)
        {
            continue;
        }

        auto& par = ecs.get<component::Parent>(n.e);
        auto& local = ecs.get<component::LocalTRS>(n.e);
        auto& world = ecs.get<component::WorldTRS>(n.e);
        auto& dirty = ecs.get<component::DirtyFlag>(n.e);

        const bool was_dirty = dirty.dirty;
        const bool must_update = n.parent_changed || was_dirty;

        if (!par.parent.has_value())
        {
            // root: world = local if needed
            if (must_update)
            {
                world.orientation = local.orientation;
                world.translation = local.translation;
                world.scale = local.scale;
            }
        }
        else
        {
            const auto& pw = ecs.get<component::WorldTRS>(*par.parent);
            if (must_update)
            {
                compose(pw, local, world);
            }
        }

        // Clear this node's dirty bit after using it
        dirty.dirty = false;

        // Children inherit "parent_changed" if this node updated
        const bool child_parent_changed = must_update;

        // Push children
        const auto& kids = ecs.get<component::Children>(n.e);
        for (Entity c : kids.entities)
        {
            // We only need to visit a child if either:
            // - its parent changed, or
            // - the child is itself dirty.
            const bool child_dirty = ecs.get<component::DirtyFlag>(c).dirty;
            if (child_parent_changed || child_dirty)
            {
                stack.push_back({ c, child_parent_changed });
            }
        }
    }
}
// WorldTRS update pass. Call once per frame before collecting draw data.
void update_world_trs()
{
    details::ECS& ecs = details::get_ecs();

    // 1) Collect dirty entities
    std::vector<Entity> dirty;
    dirty.reserve(256);
    ecs.for_each<component::DirtyFlag>(
        [&](Entity e, const component::DirtyFlag& f)
        {
            if (f.dirty)
            {
                dirty.push_back(e);
            }
        });

    if (dirty.empty())
    {
        return;
    }

    // 2) For each dirty entity, climb to the highest dirty ancestor whose parent is clean or null
    auto parent_of = [&](Entity e) -> std::optional<Entity> { return ecs.get<component::Parent>(e).parent; };
    auto is_dirty = [&](Entity e) -> bool { return ecs.get<component::DirtyFlag>(e).dirty; };

    std::vector<Entity> tops;
    tops.reserve(dirty.size());

    for (Entity e : dirty)
    {
        Entity top = e;
        while (true)
        {
            auto p = parent_of(top);
            if (!p.has_value())
            {
                break;    // reached root
            }
            if (!is_dirty(*p))
            {
                break;    // parent clean -> stop here
            }
            top = *p;    // climb further up
        }
        tops.push_back(top);
    }

    // Deduplicate tops
    std::sort(tops.begin(), tops.end(), [](Entity a, Entity b) { return a.to_underlying() < b.to_underlying(); });
    tops.erase(std::unique(tops.begin(), tops.end(), [](Entity a, Entity b) { return a.to_underlying() == b.to_underlying(); }),
               tops.end());

    // 3) Recompute each dirty subtree starting from its top
    for (Entity top : tops)
    {
        // If top has a clean parent, that parent's world is already valid.
        // Force update at 'top' because either its parent changed earlier in this pass or top itself is dirty.
        recompute_subtree(ecs, top, /*parent_changed=*/true);
    }
}
//
//
void add_mesh_baseline_components(Entity e, const asl::TRS& local, std::optional<Entity> parent = std::nullopt)
{
    details::ECS& ecs = details::get_ecs();

    ecs.emplace<component::Parent>(e, parent);
    ecs.emplace<component::Children>(e);

    ecs.emplace<component::LocalTRS>(e, local.orientation, local.translation, local.scale);
    ecs.emplace<component::WorldTRS>(e, local.orientation, local.translation, local.scale);
    ecs.emplace<component::DirtyFlag>(e, true);
}
void make_submesh_entities(component::Model& model, std::span<const std::int32_t> ids)
{
    details::ECS& ecs = details::get_ecs();

    std::unordered_map<const asl::NodeView*, Entity> entityTracker{};
    std::size_t meshIndex{};
    auto visitor = [&model, ids, &ecs, &entityTracker, &meshIndex](const asl::NodeView* pParent, const asl::NodeView* pChild)
    {
        auto [kvPair, inserted] = entityTracker.try_emplace(pChild, ecs.make_entity());
        ODIN_ASSERT(inserted);

        Entity& e = kvPair->second;

        std::optional<Entity> parent;
        if (pParent == nullptr)
        {
            ODIN_ASSERT(!pChild->geometry);    // Enforce dummy root
            model.root = e;
        }
        else
        {
            ODIN_ASSERT(pChild->geometry);    // Enforce that all children has geometry
            model.nodes.emplace_back(pChild->geometry->name, e);
            parent = std::make_optional(entityTracker.at(pParent));
        }

        const asl::TRS& local = pChild->local;
        add_mesh_baseline_components(e, local, parent);

        if (parent.has_value())
        {
            auto& children = ecs.get<component::Children>(parent.value());
            children.entities.emplace_back(e);
        }

        if (pChild->geometry)
        {
            auto& geometry = ecs.emplace<component::Geometry>(e);

            for (std::size_t i = 0; i < pChild->geometry->renderables.size(); ++i)
            {
                ODIN_ASSERT(meshIndex < ids.size());
                std::int32_t id = ids[meshIndex++];
                geometry.meshIDs.emplace_back(id);
            }
        }
    };

    const asl::ModelHandle& handle = model.handle;
    std::shared_ptr<const asl::SceneGraph> pGraph = handle.acquire();
    pGraph->dfs(visitor);

    using Node = std::pair<std::string, Entity>;
    std::sort(std::begin(model.nodes), std::end(model.nodes), [](const Node& lhs, const Node& rhs) { return lhs.first < rhs.first; });

    ODIN_ASSERT(meshIndex == ids.size());
}
[[nodiscard]] std::vector<graphics::InstanceInfo> collect_instance_infos()
{
    details::ECS& ecs = details::get_ecs();

    std::vector<graphics::InstanceInfo> infos{};
    infos.reserve(2048);
    ecs.for_each<component::Geometry, component::WorldTRS>(
        [&infos]([[maybe_unused]] Entity e, const component::Geometry& geometry, const component::WorldTRS& trs)
        {
            for (auto&& id : geometry.meshIDs)
            {
                ODIN_ASSERT(id != MESH_DUMMY_SENTINEL);
                infos.emplace_back(trs.orientation, trs.translation, trs.scale, id);
            }
        });

    return infos;
}
}    // namespace
namespace odin
{
class Odin::Impl
{
    enum class State
    {
        begin,
        graphics,
        end
    };
public:
    Impl(OdinInfo info)
        : m_State{ State::end }
        , m_FilepathResolver{ FilepathResolver::get(info.argc, info.argv) }
        , m_Assets{}
        , m_Wnd{ info.applicationName, info.windowInfo }
        , m_Gfx{ info, m_Wnd }
        , m_Quit{ false }
    {
        details::ECS& ecs = details::get_ecs();
        ecs.emplace_global_state<state::Input>();
    }
public:
    void begin_frame()
    {
        ODIN_ASSERT(m_State == State::end);
        m_State = State::begin;

        std::span<const window::InputEvent> events = m_Wnd.poll_input();
        m_Quit = poll_input(events);

        register_new_scenegraphs();
    }
    void render()
    {
        ODIN_ASSERT(m_State == State::begin);
        m_State = State::graphics;

        update_world_trs();
        std::vector<graphics::InstanceInfo> infos = collect_instance_infos();
        m_Gfx.draw(infos);
    }
    void end_frame()
    {
        ODIN_ASSERT(m_State == State::graphics);
        m_State = State::end;

        // Request unused mesh eviction here
    }
    bool running() const { return !m_Quit; }
private:
    void register_new_scenegraphs()
    {
        details::ECS& ecs = details::get_ecs();
        auto register_if_needed = [this, &ecs]([[maybe_unused]] Entity e, component::Model& model)
        {
            if (!model.handle)
            {
                model.handle = m_Assets.model_handle(model.filepath);
            }

            const asl::ModelHandle& handle = model.handle;
            if (!m_Gfx.is_registered(handle))
            {
                std::vector<std::int32_t> ids = m_Gfx.register_model(handle);
                make_submesh_entities(model, ids);
            }
        };
        ecs.for_each<component::Model>(register_if_needed);
    }
private:
    State m_State;
    FilepathResolver& m_FilepathResolver;
    asl::AssetRegistry m_Assets;
    window::Window m_Wnd;
    graphics::Graphics m_Gfx;
    bool m_Quit;
};
//
//
Odin::Odin(OdinInfo info)
    : m_pImpl{ std::make_unique<Impl>(std::move(info)) }
{}
Odin::~Odin() = default;
Odin::Odin(Odin&& other) noexcept = default;
Odin& Odin::operator=(Odin&& other) noexcept = default;
void Odin::begin_frame()
{
    m_pImpl->begin_frame();
}
void Odin::render()
{
    m_pImpl->render();
}
void Odin::end_frame()
{
    m_pImpl->end_frame();
}
bool Odin::running() const
{
    return m_pImpl->running();
}
}    // namespace odin
