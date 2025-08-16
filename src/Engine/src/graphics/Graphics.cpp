//
// Created by qwerty on 11/08/2025.
//
#include "Graphics.hpp"

#include "debug/Logger.hpp"
#include "FrameHandler.hpp"
#include "Presenter.hpp"
#include "VulkanContext.hpp"
#include "window/Window.hpp"
//
//
namespace
{
[[nodiscard]] VkApplicationInfo make_application_info(std::string_view name)
{
    return VkApplicationInfo{ .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                              .pNext = nullptr,
                              .pApplicationName = name.data(),
                              .applicationVersion = VK_MAKE_VERSION(1, 33, 7),
                              .pEngineName = "Odin",
                              .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                              .apiVersion = VK_API_VERSION_1_4 };
}
}    // namespace
namespace odin::graphics
{
class Graphics::Impl
{
public:
    [[nodiscard]] static Graphics::Impl make_graphics(const odin::OdinInfo& info)
    {
        // The circular dependencies for initialization is nuts..
        // So keep this as a stand alone function for clarity and then move everything into place

        // Make window
        window::Window wnd{ info.applicationName, info.windowInfo };
        // Make instance
        vk::Instance instance{ make_application_info(info.applicationName), wnd.required_extensions() };
        // Make surface
        vk::Surface surface{ wnd, instance.handle() };
        // Make physdev
        vk::PhysicalDevice physicalDevice{ instance, surface };
        // Make q fams
        vk::QueueFamilies queueFamilies{ physicalDevice, surface };
        // Make dev
        vk::Device device{ physicalDevice, queueFamilies };
        // Make FrameHandler
        FrameHandler frameHandler{ device.handle(), queueFamilies.graphics(), queueFamilies.compute() };
        // Make Presenter
        Presenter presenter{ device, physicalDevice, std::move(surface) };
        // Make Renderer
        //Renderer renderer{};

        // Make Vulkan Context
        VulkanContext context{ std::move(instance), std::move(physicalDevice), std::move(queueFamilies), std::move(device) };
        // return Graphics as r value
        return Graphics::Impl{ std::move(wnd),
                               std::move(context),
                               std::move(frameHandler),
                               std::move(presenter) /*, std::move(renderer)*/ };
    }
public:
    // TODO: give better name
    void draw()
    {
        using ColorAttachment = vk::resource::ImageViewRef;

        FrameContext frame = m_FrameHandler.start_frame();

        std::optional<ColorAttachment> colorAttach = m_Presenter.acquire_color_attachment(frame.colorAttachmentAvailable);
        if (colorAttach)
        {
            // Do uploading
            // uploadermanager.upload()
            // Do rendering stuff
            // rednderer.render()

            const vk::QueueFamilies& queues = m_Context.queue_families();
            if (!m_Presenter.present(queues.present(), frame.renderingFinished))
            {
                LOG_WARN("Failed to present color attachment.");
            }
        }
    }
private:
    Impl(window::Window window, VulkanContext context, FrameHandler frameHandler, Presenter presenter /*, Renderer&& renderer */)
        : m_Wnd{ std::move(window) }
        , m_Context{ std::move(context) }
        , m_FrameHandler{ std::move(frameHandler) }
        , m_Presenter{ std::move(presenter) }
    {}
private:
    window::Window m_Wnd;
    VulkanContext m_Context;
    FrameHandler m_FrameHandler;
    Presenter m_Presenter;
};
// Pimpl
Graphics::Graphics(const OdinInfo& info)
    : m_pImpl{ std::make_unique<Impl>(Graphics::Impl::make_graphics(info)) }
{}
Graphics::~Graphics() = default;
Graphics::Graphics(Graphics&& other) noexcept = default;
Graphics& Graphics::operator=(Graphics&& other) noexcept = default;
}    // namespace odin::graphics
