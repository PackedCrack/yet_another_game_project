#include "Input.hpp"

#include "ECS.hpp"
//
//
namespace odin
{
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
[[nodiscard]] bool poll_input(std::span<const window::InputEvent> events)
{
    details::ECS& ecs = details::get_ecs();
    auto& input = ecs.global_state<state::Input>();
    input.mouseMotion.deltaX = 0.0f;
    input.mouseMotion.deltaY = 0.0f;

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
            return quit.quit;
        }
        }
    }

    return false;
}
}    // namespace odin
