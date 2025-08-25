//
// Created by qwerty on 10/08/2025.
//
#include "Window.hpp"

#include "sdl_defines.hpp"
// common
#include <common.hpp>
// sdl
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
//
//
// Scancode validation
namespace
{
using namespace odin;
static_assert(std::to_underlying(Scancode::A) == SDL_SCANCODE_A);
static_assert(std::to_underlying(Scancode::B) == SDL_SCANCODE_B);
static_assert(std::to_underlying(Scancode::C) == SDL_SCANCODE_C);
static_assert(std::to_underlying(Scancode::D) == SDL_SCANCODE_D);
static_assert(std::to_underlying(Scancode::E) == SDL_SCANCODE_E);
static_assert(std::to_underlying(Scancode::F) == SDL_SCANCODE_F);
static_assert(std::to_underlying(Scancode::G) == SDL_SCANCODE_G);
static_assert(std::to_underlying(Scancode::H) == SDL_SCANCODE_H);
static_assert(std::to_underlying(Scancode::I) == SDL_SCANCODE_I);
static_assert(std::to_underlying(Scancode::K) == SDL_SCANCODE_K);
static_assert(std::to_underlying(Scancode::L) == SDL_SCANCODE_L);
static_assert(std::to_underlying(Scancode::M) == SDL_SCANCODE_M);
static_assert(std::to_underlying(Scancode::N) == SDL_SCANCODE_N);
static_assert(std::to_underlying(Scancode::O) == SDL_SCANCODE_O);
static_assert(std::to_underlying(Scancode::P) == SDL_SCANCODE_P);
static_assert(std::to_underlying(Scancode::Q) == SDL_SCANCODE_Q);
static_assert(std::to_underlying(Scancode::R) == SDL_SCANCODE_R);
static_assert(std::to_underlying(Scancode::S) == SDL_SCANCODE_S);
static_assert(std::to_underlying(Scancode::T) == SDL_SCANCODE_T);
static_assert(std::to_underlying(Scancode::U) == SDL_SCANCODE_U);
static_assert(std::to_underlying(Scancode::V) == SDL_SCANCODE_V);
static_assert(std::to_underlying(Scancode::W) == SDL_SCANCODE_W);
static_assert(std::to_underlying(Scancode::X) == SDL_SCANCODE_X);
static_assert(std::to_underlying(Scancode::Y) == SDL_SCANCODE_Y);
static_assert(std::to_underlying(Scancode::Z) == SDL_SCANCODE_Z);

static_assert(std::to_underlying(Scancode::one) == SDL_SCANCODE_1);
static_assert(std::to_underlying(Scancode::two) == SDL_SCANCODE_2);
static_assert(std::to_underlying(Scancode::three) == SDL_SCANCODE_3);
static_assert(std::to_underlying(Scancode::four) == SDL_SCANCODE_4);
static_assert(std::to_underlying(Scancode::five) == SDL_SCANCODE_5);
static_assert(std::to_underlying(Scancode::six) == SDL_SCANCODE_6);
static_assert(std::to_underlying(Scancode::seven) == SDL_SCANCODE_7);
static_assert(std::to_underlying(Scancode::eight) == SDL_SCANCODE_8);
static_assert(std::to_underlying(Scancode::nine) == SDL_SCANCODE_9);
static_assert(std::to_underlying(Scancode::zero) == SDL_SCANCODE_0);

static_assert(std::to_underlying(Scancode::return_) == SDL_SCANCODE_RETURN);
static_assert(std::to_underlying(Scancode::escape) == SDL_SCANCODE_ESCAPE);
static_assert(std::to_underlying(Scancode::backspace) == SDL_SCANCODE_BACKSPACE);
static_assert(std::to_underlying(Scancode::tab) == SDL_SCANCODE_TAB);
static_assert(std::to_underlying(Scancode::space) == SDL_SCANCODE_SPACE);

static_assert(std::to_underlying(Scancode::minus) == SDL_SCANCODE_MINUS);
static_assert(std::to_underlying(Scancode::equals) == SDL_SCANCODE_EQUALS);
static_assert(std::to_underlying(Scancode::leftBracket) == SDL_SCANCODE_LEFTBRACKET);
static_assert(std::to_underlying(Scancode::rightBracket) == SDL_SCANCODE_RIGHTBRACKET);
static_assert(std::to_underlying(Scancode::backslash) == SDL_SCANCODE_BACKSLASH);
static_assert(std::to_underlying(Scancode::semicolon) == SDL_SCANCODE_SEMICOLON);
static_assert(std::to_underlying(Scancode::apostrophe) == SDL_SCANCODE_APOSTROPHE);
static_assert(std::to_underlying(Scancode::grave) == SDL_SCANCODE_GRAVE);
static_assert(std::to_underlying(Scancode::comma) == SDL_SCANCODE_COMMA);
static_assert(std::to_underlying(Scancode::period) == SDL_SCANCODE_PERIOD);
static_assert(std::to_underlying(Scancode::slash) == SDL_SCANCODE_SLASH);

static_assert(std::to_underlying(Scancode::capslock) == SDL_SCANCODE_CAPSLOCK);

static_assert(std::to_underlying(Scancode::f1) == SDL_SCANCODE_F1);
static_assert(std::to_underlying(Scancode::f2) == SDL_SCANCODE_F2);
static_assert(std::to_underlying(Scancode::f3) == SDL_SCANCODE_F3);
static_assert(std::to_underlying(Scancode::f4) == SDL_SCANCODE_F4);
static_assert(std::to_underlying(Scancode::f5) == SDL_SCANCODE_F5);
static_assert(std::to_underlying(Scancode::f6) == SDL_SCANCODE_F6);
static_assert(std::to_underlying(Scancode::f7) == SDL_SCANCODE_F7);
static_assert(std::to_underlying(Scancode::f8) == SDL_SCANCODE_F8);
static_assert(std::to_underlying(Scancode::f9) == SDL_SCANCODE_F9);
static_assert(std::to_underlying(Scancode::f10) == SDL_SCANCODE_F10);
static_assert(std::to_underlying(Scancode::f11) == SDL_SCANCODE_F11);
static_assert(std::to_underlying(Scancode::f12) == SDL_SCANCODE_F12);

static_assert(std::to_underlying(Scancode::home) == SDL_SCANCODE_HOME);
static_assert(std::to_underlying(Scancode::pageDown) == SDL_SCANCODE_PAGEDOWN);
static_assert(std::to_underlying(Scancode::delete_) == SDL_SCANCODE_DELETE);
static_assert(std::to_underlying(Scancode::end) == SDL_SCANCODE_END);
static_assert(std::to_underlying(Scancode::pageDown) == SDL_SCANCODE_PAGEDOWN);
static_assert(std::to_underlying(Scancode::right) == SDL_SCANCODE_RIGHT);
static_assert(std::to_underlying(Scancode::left) == SDL_SCANCODE_LEFT);
static_assert(std::to_underlying(Scancode::down) == SDL_SCANCODE_DOWN);
static_assert(std::to_underlying(Scancode::up) == SDL_SCANCODE_UP);

static_assert(std::to_underlying(Scancode::numLockClear) == SDL_SCANCODE_NUMLOCKCLEAR);

static_assert(std::to_underlying(Scancode::keypadDivide) == SDL_SCANCODE_KP_DIVIDE);
static_assert(std::to_underlying(Scancode::keypadMultiply) == SDL_SCANCODE_KP_MULTIPLY);
static_assert(std::to_underlying(Scancode::keypadMinus) == SDL_SCANCODE_KP_MINUS);
static_assert(std::to_underlying(Scancode::keypadPlus) == SDL_SCANCODE_KP_PLUS);
static_assert(std::to_underlying(Scancode::keypadEnter) == SDL_SCANCODE_KP_ENTER);
static_assert(std::to_underlying(Scancode::keypadOne) == SDL_SCANCODE_KP_1);
static_assert(std::to_underlying(Scancode::keypadTwo) == SDL_SCANCODE_KP_2);
static_assert(std::to_underlying(Scancode::keypadThree) == SDL_SCANCODE_KP_3);
static_assert(std::to_underlying(Scancode::keypadFour) == SDL_SCANCODE_KP_4);
static_assert(std::to_underlying(Scancode::keypadFive) == SDL_SCANCODE_KP_5);
static_assert(std::to_underlying(Scancode::keypadSix) == SDL_SCANCODE_KP_6);
static_assert(std::to_underlying(Scancode::keypadSeven) == SDL_SCANCODE_KP_7);
static_assert(std::to_underlying(Scancode::keypadEight) == SDL_SCANCODE_KP_8);
static_assert(std::to_underlying(Scancode::keypadNine) == SDL_SCANCODE_KP_9);
static_assert(std::to_underlying(Scancode::keypadZero) == SDL_SCANCODE_KP_0);

static_assert(std::to_underlying(Scancode::leftCtrl) == SDL_SCANCODE_LCTRL);
static_assert(std::to_underlying(Scancode::leftShift) == SDL_SCANCODE_LSHIFT);
static_assert(std::to_underlying(Scancode::leftAlt) == SDL_SCANCODE_LALT);
static_assert(std::to_underlying(Scancode::leftGui) == SDL_SCANCODE_LGUI);
static_assert(std::to_underlying(Scancode::rightCtrl) == SDL_SCANCODE_RCTRL);
static_assert(std::to_underlying(Scancode::rightShift) == SDL_SCANCODE_RSHIFT);
static_assert(std::to_underlying(Scancode::rightAlt) == SDL_SCANCODE_RALT);
static_assert(std::to_underlying(Scancode::rightGui) == SDL_SCANCODE_RGUI);
}    // namespace
// Keymodifer validation
namespace
{
static_assert(std::to_underlying(KeyModifier::none) == SDL_KMOD_NONE);
static_assert(std::to_underlying(KeyModifier::leftShift) == SDL_KMOD_LSHIFT);
static_assert(std::to_underlying(KeyModifier::rightShift) == SDL_KMOD_RSHIFT);
static_assert(std::to_underlying(KeyModifier::leftCtrl) == SDL_KMOD_LCTRL);
static_assert(std::to_underlying(KeyModifier::rightCtrl) == SDL_KMOD_RCTRL);
static_assert(std::to_underlying(KeyModifier::leftAlt) == SDL_KMOD_LALT);
static_assert(std::to_underlying(KeyModifier::rightAlt) == SDL_KMOD_RALT);
static_assert(std::to_underlying(KeyModifier::leftGui) == SDL_KMOD_LGUI);
static_assert(std::to_underlying(KeyModifier::rightGui) == SDL_KMOD_RGUI);
static_assert(std::to_underlying(KeyModifier::altGr) == SDL_KMOD_MODE);
static_assert(std::to_underlying(KeyModifier::ctrl) == SDL_KMOD_CTRL);
static_assert(std::to_underlying(KeyModifier::shift) == SDL_KMOD_SHIFT);
static_assert(std::to_underlying(KeyModifier::alt) == SDL_KMOD_ALT);
static_assert(std::to_underlying(KeyModifier::gui) == SDL_KMOD_GUI);
}    // namespace
// Mouse button validation
namespace
{
static_assert(std::to_underlying(MouseButton::left) == SDL_BUTTON_LEFT);
static_assert(std::to_underlying(MouseButton::right) == SDL_BUTTON_RIGHT);
static_assert(std::to_underlying(MouseButton::middle) == SDL_BUTTON_MIDDLE);
static_assert(std::to_underlying(MouseButton::extra1) == SDL_BUTTON_X1);
static_assert(std::to_underlying(MouseButton::extra2) == SDL_BUTTON_X2);
}    // namespace
//
//
//
//
namespace
{
using namespace odin::window;
//
//
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
[[nodiscard]] InputEvent make_mouse_motion_event(const SDL_MouseMotionEvent& motion)
{
    SDL_MouseButtonFlags buttonFlags = motion.state;

    MouseMotionEvent event{};
    event.x = motion.x;
    event.y = motion.y;
    event.deltaX = motion.xrel;
    event.deltaY = motion.yrel;
    event.leftPressed = (buttonFlags & SDL_BUTTON_LEFT) ? true : false;
    event.rightPressed = (buttonFlags & SDL_BUTTON_RIGHT) ? true : false;
    event.middlePressed = (buttonFlags & SDL_BUTTON_MIDDLE) ? true : false;
    event.extra1Pressed = (buttonFlags & SDL_BUTTON_X1) ? true : false;
    event.extra2Pressed = (buttonFlags & SDL_BUTTON_X2) ? true : false;

    return InputEvent{ .type = EventType::mouseMotion, .event = event };
}
[[nodiscard]] InputEvent make_mouse_click_event(const SDL_MouseButtonEvent& button)
{
    MouseClickEvent event{};
    event.button = MouseButton(button.button);
    event.x = button.x;
    event.y = button.y;
    event.pressed = button.down;
    event.numClicks = button.clicks;

    return InputEvent{ .type = EventType::mouseClick, .event = event };
}
[[nodiscard]] InputEvent make_keyboard_event(const SDL_KeyboardEvent& key)
{
    KeyboardEvent event{};
    event.code = Scancode(key.scancode);
    event.modifier = KeyModifier(key.mod);
    event.pressed = key.down;
    event.repeated = key.repeat;

    return InputEvent{ .type = EventType::keyboard, .event = event };
}
[[nodiscard]] InputEvent make_quit_event(const SDL_QuitEvent& quit)
{
    QuitEvent event{};
    event.quit = true;

    return InputEvent{ .type = EventType::quit, .event = event };
}
}    // namespace
namespace odin::window
{
class Window::Impl
{
public:
    explicit Impl(std::string_view title, const WindowInfo& info)
        : m_pWindow{ nullptr }
        , m_Settings{ info }
        , m_Events{}
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
        , m_Events{ std::move(other.m_Events) }
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
            m_Events = std::move(other.m_Events);
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
    std::span<const InputEvent> poll_input()
    {
        m_Events.clear();

        SDL_Event event{};
        while (SDL_PollEvent(std::addressof(event)))
        {
            switch (event.type)
            {
            case SDL_EVENT_MOUSE_MOTION:
            {
                m_Events.emplace_back(make_mouse_motion_event(event.motion));
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                m_Events.emplace_back(make_mouse_click_event(event.button));
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                m_Events.emplace_back(make_mouse_click_event(event.button));
                break;
            }
            case SDL_EVENT_KEY_DOWN:
            {
                m_Events.emplace_back(make_keyboard_event(event.key));
                break;
            }
            case SDL_EVENT_KEY_UP:
            {
                m_Events.emplace_back(make_keyboard_event(event.key));
                break;
            }
            case SDL_EVENT_QUIT:
                m_Events.emplace_back(make_quit_event(event.quit));
                break;
            }
        }

        return common::to_span(m_Events);
    }
    [[nodiscard]] std::vector<std::string_view> extensions() const { return get_instance_extensions(); }
    [[nodiscard]] std::function<void*(void*)> make_create_surface()
    {
        // void* So vulkan concrete handles do not leak from the pimpl
        // Creating the surface in window so SDL functions don't leak from the pimpl
        return [this](void* pInstance)
        {
            ODIN_ASSERT(pInstance != nullptr);

            auto instance = static_cast<VkInstance>(pInstance);
            VkSurfaceKHR surface{};
            SDL_CHECK(SDL_Vulkan_CreateSurface(m_pWindow, instance, nullptr, std::addressof(surface)), "Failed to create Vulkan Surface.");

            return surface;
        };
    }
private:
    WindowInfo m_Settings;
    SDL_Window* m_pWindow;
    std::vector<InputEvent> m_Events;
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
std::function<void*(void*)> Window::make_create_surface()
{
    return m_pImpl->make_create_surface();
}
std::span<const InputEvent> Window::poll_input()
{
    return m_pImpl->poll_input();
}
}    // namespace odin::window
