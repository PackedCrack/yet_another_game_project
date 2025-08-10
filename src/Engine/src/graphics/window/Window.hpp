#pragma once

int func();

namespace gfx::window
{
class Window
{
	struct Impl;
public:
	Window(std::string_view title);
	~Window() = default;
	Window(const Window& other) = delete;
	Window(Window&& other) = default;
	Window& operator=(const Window& other) = delete;
	Window& operator=(Window&& other) = default;
public:
	void toggle_borderless();
	void toggle_fullscreen();
	void toggle_mouse_grab();
private:
	std::unique_ptr<Window::Impl> m_pImpl;
};
}	// namespace gfx::window