//
// Created by qwerty on 10/08/2025.
//
#include "Window.hpp"

#include "sdl_defines.hpp"
// sdl
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
//
//
namespace
{
[[nodiscard]] SDL_WindowFlags make_window_flags(const odin::WindowInfo& info)
{
    SDL_WindowFlags flags{};

    if (info.borderless)
    {
        flags |= SDL_WINDOW_BORDERLESS;
    }
    if (info.fullscreen)
    {
        flags |= SDL_WINDOW_FULLSCREEN;
    }
    if (info.mouseGrab)
    {
        flags |= SDL_WINDOW_MOUSE_GRABBED;
    }

    return flags;
}
[[nodiscard]] std::vector<std::string_view> get_instance_extensions()
{
    std::uint32_t count{};
    const char* const* pExtensions = SDL_Vulkan_GetInstanceExtensions(std::addressof(count));
    std::vector<std::string_view> extensions{};
    extensions.resize(count);

    auto c = static_cast<std::size_t>(count);
    for (std::size_t i = 0; i < c; ++i)
    {
        extensions[i] = std::string_view{ pExtensions[i] };
    }

    return extensions;
}
}    // namespace
namespace odin::graphics::window
{
class Window::Impl
{
public:
    explicit Impl(std::string_view title, const WindowInfo& info)
        : m_pWindow{ nullptr }
        , m_Settings{ info }
    {
        SDL_CHECK(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_GAMEPAD), "SDL Failed to initialize.");

        SDL_WindowFlags flags = SDL_WINDOW_VULKAN;
        flags |= make_window_flags(m_Settings);

        // !!!
        // This function should only be called on the main thread.
        // https://wiki.libsdl.org/SDL3/SDL_CreateWindow
        m_pWindow = SDL_CreateWindow(title.data(), get_width(m_Settings.resolution), get_height(m_Settings.resolution), flags);
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
        : m_pWindow{ nullptr }
        , m_Settings{ other.m_Settings }
    {
        std::swap(m_pWindow, other.m_pWindow);
    }
    Impl& operator=(const Impl& other) = delete;
    Impl& operator=(Impl&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            m_pWindow = std::exchange(other.m_pWindow, nullptr);
            m_Settings = other.m_Settings;
        }

        return *this;
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
    [[nodiscard]] std::vector<std::string_view> extensions() const { return get_instance_extensions(); }
    [[nodiscard]] VkSurfaceKHR make_surface(vk::InstanceRef instance)
    {
        VkSurfaceKHR surface{};
        SDL_CHECK(SDL_Vulkan_CreateSurface(m_pWindow, instance.handle, nullptr, std::addressof(surface)),
                  "Failed to create Vulkan Surface.");
        return surface;
    }
private:
    WindowInfo m_Settings;
    SDL_Window* m_pWindow;
};
//
//
Window::Window(std::string_view title, const WindowInfo& info)
    : m_pImpl{ std::make_unique<Impl>(title, info) }
{}
Window::~Window() = default;
Window::Window(Window&& other) noexcept = default;
Window& Window::operator=(Window&& other) noexcept = default;
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
std::vector<std::string_view> Window::required_extensions() const
{
    return m_pImpl->extensions();
}
VkSurfaceKHR Window::make_surface(vk::InstanceRef instance)
{
    return m_pImpl->make_surface(instance);
}
}    // namespace odin::graphics::window
