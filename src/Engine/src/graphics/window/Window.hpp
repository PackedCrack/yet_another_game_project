//
// Created by qwerty on 10/08/2025.
//
#pragma once

#include "../../OdinInfo.hpp"
#include "../vk/Instance.hpp"
//
//
namespace odin::graphics::window
{
class Window
{
    class Impl;
public:
    Window(std::string_view title, const WindowInfo& info);
    ~Window();
    Window(const Window& other) = delete;
    Window(Window&& other) noexcept;
    Window& operator=(const Window& other) = delete;
    Window& operator=(Window&& other) noexcept;
public:
    void toggle_borderless();
    void toggle_fullscreen();
    void toggle_mouse_grab();
    [[nodiscard]] std::vector<std::string_view> required_extensions() const;
    [[nodiscard]] VkSurfaceKHR make_surface(vk::InstanceRef instance);
private:
    std::unique_ptr<Window::Impl> m_pImpl;
};
}    // namespace odin::graphics::window
