//
// Created by qwerty on 11/08/2025.
//
#include "Graphics.hpp"

#include "debug/Logger.hpp"
#include "vk/vulkan_defines.hpp"
#include "window/Window.hpp"
#include "FrameHandler.hpp"
#include "Presenter.hpp"
#include "VulkanContext.hpp"
#include "Renderer.hpp"
#include "TransferManager.hpp"
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
    [[nodiscard]] static std::unique_ptr<Impl> make_graphics(const odin::OdinInfo& info)
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
        // Make FrameResources
        // Make Presenter
        Presenter presenter{ device, physicalDevice, std::move(surface) };
        // Make Renderer
        Renderer renderer{ /*std::move(frameResources)*/ };
        // Make TransferManager
        TransferManager transferManager{ device.handle(), queueFamilies.transfer() };

        // Make Vulkan Context
        VulkanContext context{ std::move(instance), std::move(physicalDevice), std::move(queueFamilies), std::move(device) };
        // return Graphics as r value
        return std::make_unique<Impl>(std::move(wnd),
                                      std::move(context),
                                      std::move(frameHandler),
                                      std::move(presenter),
                                      std::move(renderer),
                                      std::move(transferManager));
    }
public:
    Impl(window::Window window,
         VulkanContext context,
         FrameHandler frameHandler,
         Presenter presenter,
         Renderer renderer,
         TransferManager transferManager)
        : m_Wnd{ std::move(window) }
        , m_Context{ std::move(context) }
        , m_FrameHandler{ std::move(frameHandler) }
        , m_Presenter{ std::move(presenter) }
        , m_Renderer{ std::move(renderer) }
        , m_TransferManager{ std::move(transferManager) }
    {}
    ~Impl()
    {
        // Force wait for GPU when exiting the application..
        VkDevice device = m_Context.device().handle().handle;
        if (device != VK_NULL_HANDLE)
        {
            VK_CHECK(vkDeviceWaitIdle(device), "Failed to await for GPU to idle when exiting.");
        }
    }
    Impl(Impl&& other) noexcept
        : m_Wnd{ std::move(other.m_Wnd) }
        , m_Context{ std::move(other.m_Context) }
        , m_FrameHandler{ std::move(other.m_FrameHandler) }
        , m_Presenter{ std::move(other.m_Presenter) }
        , m_Renderer{ std::move(other.m_Renderer) }
        , m_TransferManager{ std::move(other.m_TransferManager) }
    {}
    Impl& operator=(Impl&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            m_Wnd = std::move(other.m_Wnd);
            m_Context = std::move(other.m_Context);
            m_FrameHandler = std::move(other.m_FrameHandler);
            m_Presenter = std::move(other.m_Presenter);
            m_Renderer = std::move(other.m_Renderer);
            m_TransferManager = std::move(other.m_TransferManager);
        }
        return *this;
    }
public:
    void draw()
    {
        FrameContext frame = m_FrameHandler.start_frame();

        const vk::CommandBuffer& transferBuffer = frame.transferBuffer.get();
        std::optional<TransferEpoch> transferEpoch = m_TransferManager.submit_transfer(transferBuffer);

        std::optional<ColorAttachment> colorAttach = m_Presenter.acquire_color_attachment(frame.colorAttachmentReady);
        if (colorAttach)
        {
            // Do uploading
            // uploadermanager.upload()

            // Do rendering stuff
            vk::QueueView graphicsQ = m_Context.queue_families().graphics();
            m_Renderer.render_frame(colorAttach.value(), graphicsQ, frame);

            const vk::QueueFamilies& queues = m_Context.queue_families();
            if (!m_Presenter.present(queues.present(), frame.graphicsFinished))
            {
                LOG_WARN("Failed to present color attachment.");
            }
        }
    }
private:
    window::Window m_Wnd;
    VulkanContext m_Context;
    FrameHandler m_FrameHandler;
    Presenter m_Presenter;
    Renderer m_Renderer;
    TransferManager m_TransferManager;
};
// Pimpl
Graphics::Graphics(const OdinInfo& info)
    //: m_pImpl{ std::make_unique<Impl>(Graphics::Impl::make_graphics(info)) }
    : m_pImpl{ Impl::make_graphics(info) }
{}
Graphics::~Graphics() = default;
Graphics::Graphics(Graphics&& other) noexcept = default;
Graphics& Graphics::operator=(Graphics&& other) noexcept = default;
void Graphics::draw()
{
    m_pImpl->draw();
}
}    // namespace odin::graphics
