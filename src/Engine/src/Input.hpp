//
// Created by qwerty on 07/11/2025.
//
#pragma once

#include "state/Input.hpp"
#include "window/Window.hpp"
//
//
namespace odin
{
template<typename event_t>
[[nodiscard]] const event_t& to_concrete_event(const window::InputEvent& event);
[[nodiscard]] state::Input& store_keyboard_event(state::Input& state, const window::InputEvent& event);
[[nodiscard]] state::Input& store_mouse_click_event(state::Input& state, const window::InputEvent& event);
[[nodiscard]] state::Input& store_mouse_motion_event(state::Input& state, const window::InputEvent& event);
[[nodiscard]] bool poll_input(std::span<const window::InputEvent> events);
}    // namespace odin
