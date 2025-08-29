//
// Created by qwerty on 11/08/2025.
//
#include "Odin.hpp"

#include "Entity.hpp"
#include "FilepathResolver.hpp"
#include "components/LocalTRS.hpp"
#include "components/Mesh.hpp"
#include "components/Model.hpp"
#include "components/Parent.hpp"
#include "components/WorldTRS.hpp"
#include "graphics/Graphics.hpp"
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
template<typename event_t>
[[nodiscard]] const event_t& to_concrete_event(const window::InputEvent& event)
{
    const window::Event& e = event.event;
    return std::get<event_t>(e);
}
[[nodiscard]] state::Input& store_keyboard_event(state::Input& state, const window::InputEvent& event)
{
    auto& key = to_concrete_event<KeyboardEvent>(event);
    state.keyboard.code = key.code;
    state.keyboard.modifier = key.modifier;
    state.keyboard.pressed = key.pressed;
    state.keyboard.repeated = key.repeated;

    return state;
}
[[nodiscard]] state::Input& store_mouse_click_event(state::Input& state, const window::InputEvent& event)
{
    auto& click = to_concrete_event<MouseClickEvent>(event);
    state.mouseClick.button = click.button;
    state.mouseClick.numClicks = click.numClicks;
    state.mouseClick.pressed = click.pressed;
    state.mouseClick.x = click.x;
    state.mouseClick.y = click.y;

    return state;
}
[[nodiscard]] state::Input& store_mouse_motion_event(state::Input& state, const window::InputEvent& event)
{
    auto& motion = to_concrete_event<MouseMotionEvent>(event);
    state.mouseMotion.leftPressed = motion.leftPressed;
    state.mouseMotion.rightPressed = motion.rightPressed;
    state.mouseMotion.middlePressed = motion.middlePressed;
    state.mouseMotion.extra1Pressed = motion.extra1Pressed;
    state.mouseMotion.extra2Pressed = motion.extra2Pressed;

    state.mouseMotion.x = motion.x;
    state.mouseMotion.y = motion.y;
    state.mouseMotion.deltaX += motion.deltaX;
    state.mouseMotion.deltaY += motion.deltaY;

    return state;
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
        , m_Assets{}
        , m_Wnd{ info.applicationName, info.windowInfo }
        , m_Gfx{ info, m_Wnd }
        , m_ECS{ std::nullopt }
        , m_Quit{ false }
    {
        [[maybe_unused]] auto& resolver = FilepathResolver::get(info.argc, info.argv);
    }
public:
    void begin_frame()
    {
        ODIN_ASSERT(m_ECS);
        ODIN_ASSERT(m_State == State::end);
        m_State = State::begin;

        poll_input();

        register_new_scenegraphs();
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

        // Request unused mesh eviction here
    }
    std::unique_ptr<ECS> make_ecs()
    {
        auto pECS = std::make_unique<ECS>();

        // Store global state at creation
        pECS->emplace_global_state<state::Input>();

        m_ECS = std::make_optional<std::reference_wrapper<ECS>>(*pECS.get());
        return pECS;
    }
    bool running() const { return !m_Quit; }
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
    void assign_submesh_ids(const std::vector<Entity>& submeshes, const std::vector<graphics::registry::mesh::MeshID>& ids)
    {
        std::size_t index{};
        auto assign_mesh_id = [&ids, &index]([[maybe_unused]] Entity e, component::Mesh& mesh)
        {
            graphics::registry::mesh::MeshID id = ids[index++];
            mesh.id = id;
        };
        ECS& ecs = m_ECS.value();
        ecs.for_each<component::Mesh>(submeshes, assign_mesh_id);
    }
    void register_new_scenegraphs()
    {
        ECS& ecs = m_ECS->get();
        auto register_if_needed = [this, &ecs]([[maybe_unused]] Entity e, component::Model& model)
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

                std::vector<graphics::registry::mesh::MeshID> ids = m_Gfx.mesh_ids(handle);
                assign_submesh_ids(submeshes, ids);
            }
        };
        ecs.for_each<component::Model>(register_if_needed);
    }
    void poll_input()
    {
        ECS& ecs = m_ECS->get();
        auto& input = ecs.global_state<state::Input>();
        input.mouseMotion.deltaX = 0.0f;
        input.mouseMotion.deltaY = 0.0f;

        std::span<const window::InputEvent> events = m_Wnd.poll_input();
        for (auto&& event : events)
        {
            switch (event.type)
            {
            case window::EventType::keyboard:
            {
                input = store_keyboard_event(input, event);
                break;
            }
            case window::EventType::mouseClick:
            {
                input = store_mouse_click_event(input, event);
                break;
            }
            case window::EventType::mouseMotion:
            {
                input = store_mouse_motion_event(input, event);
                break;
            }
            case window::EventType::quit:
            {
                auto& quit = to_concrete_event<QuitEvent>(event);
                m_Quit = quit.quit;
                break;
            }
            }
        }
    }
private:
    State m_State;
    asl::AssetRegistry m_Assets;
    window::Window m_Wnd;
    graphics::Graphics m_Gfx;
    std::optional<std::reference_wrapper<ECS>> m_ECS;
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
std::unique_ptr<ECS> Odin::make_ecs()
{
    return m_pImpl->make_ecs();
}
bool Odin::running() const
{
    return m_pImpl->running();
}
}    // namespace odin
