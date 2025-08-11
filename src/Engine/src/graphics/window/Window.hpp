#pragma once
namespace odin::gfx::window
{
class Window
{
    class Impl;
public:
    explicit Window(std::string_view title);
    ~Window();
    Window(const Window& other) = delete;
    Window(Window&& other);
    Window& operator=(const Window& other) = delete;
    Window& operator=(Window&& other);
public:
    void toggle_borderless();
    void toggle_fullscreen();
    void toggle_mouse_grab();
private:
    std::unique_ptr<Window::Impl> m_pImpl;
};
}    // namespace odin::gfx::window
