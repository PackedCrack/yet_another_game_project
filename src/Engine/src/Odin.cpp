//
// Created by qwerty on 11/08/2025.
//
#include "Odin.hpp"

#include "Entity.hpp"
#include "FilepathResolver.hpp"
#include "Input.hpp"
#include "component/Children.hpp"
#include "component/DirtyFlag.hpp"
#include "component/LocalTRS.hpp"
#include "component/Geometry.hpp"
#include "component/Model.hpp"
#include "component/Parent.hpp"
#include "component/WorldTRS.hpp"
#include "graphics/Graphics.hpp"
#include "graphics/gpu_types.hpp"
#include "state/Input.hpp"
#include "system/transform/update_world_trs.hpp"
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
        , m_FilepathResolver{ FilepathResolver::get(info.argv) }
        , m_Assets{ m_FilepathResolver.get_asset_db() }
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

        system::transform::update_world_trs();
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
    // TODO: This should be in its own file under the system namespace
    void register_new_scenegraphs()
    {
        details::ECS& ecs = details::get_ecs();
        auto register_if_needed = [this, &ecs]([[maybe_unused]] Entity e, component::Model& model)
        {
            if (!model.handle)
            {
                model.handle = m_Assets.model_handle(model.uuid);
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
