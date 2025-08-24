//
// Created by qwerty on 11/08/2025.
//
#include "Odin.hpp"

#include "Entity.hpp"
#include "components/LocalTRS.hpp"
#include "components/Mesh.hpp"
#include "components/Model.hpp"
#include "components/Parent.hpp"
#include "components/WorldTRS.hpp"
#include "graphics/Graphics.hpp"
#include "window/Window.hpp"
// Debug
#include <debug/Logger.hpp>
#include <debug/debug_defines.hpp>
// AssetLoader
#include <assetloader/AssetRegistry.hpp>
#include <assetloader/SceneGraph.hpp>
//
//
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
        , m_Assets{}
        , m_Wnd{ info.applicationName, info.windowInfo }
        , m_Gfx{ info, m_Wnd }
    {}
public:
    void begin_frame()
    {
        ODIN_ASSERT(m_ECS);
        ODIN_ASSERT(m_State == State::end);
        m_State = State::begin;

        ECS& ecs = m_ECS->get();
        auto register_if_needed = [this, &ecs](Entity e, component::Model& model)
        {
            if (!model.handle)
            {
                model.handle = m_Assets.model_handle(model.filepath);
            }

            const asl::ModelHandle& handle = model.handle;
            if (!m_Gfx.is_registered(handle))
            {
                m_Gfx.register_model(handle);

                std::vector<Entity> submeshes = make_submesh_entities(handle);

                std::vector<graphics::MeshID> ids = m_Gfx.mesh_ids(handle);
                assign_submesh_ids(submeshes, ids);
            }
        };
        ecs.for_each<component::Model>(register_if_needed);
    }
    void render()
    {
        ODIN_ASSERT(m_State == State::begin);
        m_State = State::graphics;

        m_Gfx.draw();
    }
    void end_frame()
    {
        ODIN_ASSERT(m_State == State::graphics);
        m_State = State::end;
    }
    std::unique_ptr<ECS> make_ecs()
    {
        auto pECS = std::make_unique<ECS>();
        m_ECS = std::make_optional<std::reference_wrapper<ECS>>(*pECS.get());
        return pECS;
    }
private:
    std::vector<Entity> make_submesh_entities(const asl::ModelHandle& handle)
    {
        ECS& ecs = m_ECS.value();

        std::vector<Entity> submeshes{};
        std::unordered_map<const asl::NodeView*, Entity> entityTracker{};
        auto visitor = [&ecs, &submeshes, &entityTracker](const asl::NodeView* pParent, const asl::NodeView* pChild)
        {
            if (!entityTracker.contains(pChild))
            {
                auto [kvPair, inserted] = entityTracker.emplace(pChild, ecs.make_entity());
                Entity& child = kvPair->second;

                const asl::TRS& local = pChild->local;
                child.emplace<component::LocalTRS>(local.orientation, local.translation, local.scale);
                child.emplace<component::Mesh>();
                child.emplace<component::WorldTRS>();
                submeshes.push_back(child);

                if (pParent != nullptr)
                {
                    ODIN_ASSERT(entityTracker.contains(pParent));

                    Entity& parent = entityTracker.at(pParent);
                    child.emplace<component::Parent>(std::make_optional(parent));
                }
                else
                {
                    child.emplace<component::Parent>(std::nullopt);
                }
            }
        };

        std::shared_ptr<const asl::SceneGraph> pGraph = handle.acquire();
        pGraph->dfs(visitor);

        return submeshes;
    }
    void assign_submesh_ids(const std::vector<Entity>& submeshes, const std::vector<graphics::MeshID>& ids)
    {
        std::size_t index{};
        auto assign_mesh_id = [&ids, &index]([[maybe_unused]] Entity e, component::Mesh& mesh)
        {
            graphics::MeshID id = ids[index++];
            mesh.id = id;
        };
        ECS& ecs = m_ECS.value();
        ecs.for_each<component::Mesh>(submeshes, assign_mesh_id);
    }
private:
    State m_State;
    // AssetRegistry
    asl::AssetRegistry m_Assets;
    window::Window m_Wnd;
    graphics::Graphics m_Gfx;
    std::optional<std::reference_wrapper<ECS>> m_ECS;
};
//
//
Odin::Odin(OdinInfo info)
    : m_pImpl{ std::make_unique<Impl>(info) }
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
std::unique_ptr<ECS> Odin::make_ecs()
{
    return m_pImpl->make_ecs();
}
}    // namespace odin
