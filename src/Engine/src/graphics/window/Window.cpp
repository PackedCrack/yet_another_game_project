#include "Window.hpp"

#include "sdl_defines.hpp"
// sdl
#include "SDL3/SDL.h"
int func()
{
    return SDL_Init(0);
}
namespace
{
struct WindowSettings
{
    std::int32_t width;
    std::int32_t height;
    bool borderless;
    bool fullscreen;
    bool mouseGrab;
};
[[nodiscard]] WindowSettings make_default_settings()
{
    // TODO: Read these from userconfig
    return WindowSettings{
        .width = 1600,
        .height = 900,
        .borderless = false,
        .fullscreen = false,
        .mouseGrab = false
    };
}
[[nodiscard]] SDL_WindowFlags make_window_flags(const WindowSettings& settings)
{
    SDL_WindowFlags flags{};

    if (settings.borderless)
    {
        flags |= SDL_WINDOW_BORDERLESS;
    }
    if (settings.fullscreen)
    {
        flags |= SDL_WINDOW_FULLSCREEN;
    }
    if (settings.mouseGrab)
    {
        flags |= SDL_WINDOW_MOUSE_GRABBED;
    }

    return flags;
}
}
namespace gfx::window
{
struct Window::Impl
{
    Impl(std::string_view title)
        : m_pWindow{ nullptr }
        , m_Settings{ make_default_settings() }
    {
        SDL_CHECK(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_GAMEPAD), "SDL Failed to initialize.");
        
        SDL_WindowFlags flags = SDL_WINDOW_VULKAN;
        flags |= make_window_flags(m_Settings);

        // !!!
        // This function should only be called on the main thread.
        // https://wiki.libsdl.org/SDL3/SDL_CreateWindow
        m_pWindow = SDL_CreateWindow(title.data(), m_Settings.width, m_Settings.height, flags);
        SDL_CHECK(m_pWindow != nullptr, "SDL Failed to create a window.");
    }
    ~Impl()
    {
        if (m_pWindow != nullptr)
        {
            SDL_DestroyWindow(m_pWindow);
        }
    }
    Impl(const Impl& other) = delete;
    Impl(Impl&& other) noexcept
        : m_pWindow{ std::exchange(other.m_pWindow, nullptr) }
        , m_Settings{ other.m_Settings }
    {}
    Impl& operator=(const Impl& other) = delete;
    Impl& operator=(Impl&& other) noexcept
    {
        if (this == std::addressof(other))
        {
            return *this;
        }

        m_pWindow = std::exchange(other.m_pWindow, nullptr);
        m_Settings = other.m_Settings;
    }
public:
    void toggle_borderless()
    {
        ODIN_ASSERT(m_pWindow != nullptr);

        m_Settings.borderless = not m_Settings.borderless;
        SDL_SetWindowBordered(m_pWindow, m_Settings.borderless);
    }
    void toggle_fullscreen()
    {
        ODIN_ASSERT(m_pWindow != nullptr);

        m_Settings.fullscreen = not m_Settings.fullscreen;
        SDL_SetWindowFullscreen(m_pWindow, m_Settings.fullscreen);
    }
    void toggle_mouse_grab()
    {
        ODIN_ASSERT(m_pWindow != nullptr);

        m_Settings.mouseGrab = not m_Settings.mouseGrab;
        SDL_SetWindowMouseGrab(m_pWindow, m_Settings.mouseGrab);
    }
public:
    WindowSettings m_Settings;
    SDL_Window* m_pWindow;
};
Window::Window(std::string_view title)
    : m_pImpl{ std::make_unique<Impl>(title) }
{}
void Window::toggle_borderless()
{
    m_pImpl->toggle_borderless();
}
void Window::toggle_fullscreen()
{
    m_pImpl->toggle_fullscreen();
}
void Window::toggle_mouse_grab()
{
    m_pImpl->toggle_mouse_grab();
}
}	// namespace gfx::window