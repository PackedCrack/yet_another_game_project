#include "Graphics.hpp"

#include "VulkanContext.hpp"
#include "window/Window.hpp"
//
//
namespace odin::graphics
{
class Graphics::Impl
{
public:
    Impl(const OdinInfo& info)
        : m_Wnd{ info.applicationName, info.windowInfo }
        , m_Context{} {};
private:
    window::Window m_Wnd;
    VulkanContext m_Context;
};
// Pimpl
Graphics::Graphics(const OdinInfo& info)
    : m_pImpl{ std::make_unique<Graphics::Impl>(info) }
{}
}    // namespace odin::graphics
