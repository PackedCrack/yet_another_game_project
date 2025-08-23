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
// Debug
#include <debug/Logger.hpp>
#include <debug/debug_defines.hpp>
// AssetLoader
#include <assetloader/Model.hpp>
//
//
namespace
{
using namespace odin;
//
//
std::vector<Entity> make_submesh_entities(ECS& ecs, asl::Model& sceneGraph)
{
    std::vector<Entity> submeshes{};
    std::unordered_map<const asl::NodeView*, Entity> entityTracker{};
    auto visitor = [&ecs, &submeshes, &entityTracker](const asl::NodeView* pParent, const asl::NodeView* pChild)
    {
        if (!entityTracker.contains(pChild))
        {
            auto [kvPair, inserted] = entityTracker.emplace(pChild, ecs.make_entity());
            Entity& child = kvPair->second;

            const asl::TRS& local = pChild->local;
            child.emplace<components::LocalTRS>(local.orientation, local.translation, local.scale);
            child.emplace<components::Mesh>();
            child.emplace<components::WorldTRS>();


            submeshes.push_back(child);

            if (pParent != nullptr)
            {
                ODIN_ASSERT(entityTracker.contains(pParent));

                Entity& parent = entityTracker.at(pParent);
                child.emplace<components::Parent>(std::make_optional(parent));
            }
            else
            {
                child.emplace<components::Parent>(std::nullopt);
            }
        }
    };
    sceneGraph.dfs(visitor);
    return submeshes;
}
}    // namespace
namespace odin
{
Odin::Odin(OdinInfo info)
    : m_State{ State::end }
    , m_Gfx{ info }
{}
void Odin::begin_frame()
{
    ODIN_ASSERT(m_ECS);
    ODIN_ASSERT(m_State == State::end);
    m_State = State::begin;

    ECS& ecs = m_ECS->get();
    auto register_if_needed = [this, &ecs](Entity e, components::Model& model)
    {
        // Use AssetRegistry when it implemented
        // asl::Model sceneGraph = assetRegistry.load(model);
        asl::Model sceneGraph{ R"(C:\Users\qwerty\Documents\repos\game\resources\assets\meshes\Lantern.glb)" };
        if (!m_Gfx.is_registered(model))
        {
            m_Gfx.register_model(sceneGraph);

            std::vector<Entity> submeshes = make_submesh_entities(ecs, sceneGraph);

            m_Gfx.assign_submesh_ids(ecs, model, submeshes);
        }
    };
    ecs.for_each<components::Model>(register_if_needed);
}
void Odin::render()
{
    ODIN_ASSERT(m_State == State::begin);
    m_State = State::graphics;

    m_Gfx.draw();
}
void Odin::end_frame()
{
    ODIN_ASSERT(m_State == State::graphics);
    m_State = State::end;
}
std::unique_ptr<ECS> Odin::make_ecs()
{
    auto pECS = std::make_unique<ECS>();
    m_ECS = std::make_optional<std::reference_wrapper<ECS>>(*pECS.get());
    return pECS;
}
}    // namespace odin
