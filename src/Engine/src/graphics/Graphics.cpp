//
// Created by qwerty on 11/08/2025.
//
#include "Graphics.hpp"

#include "FrameHandler.hpp"
#include "registry/mesh/MeshRegistry.hpp"
#include "Presenter.hpp"
#include "Renderer.hpp"
#include "TransferManager.hpp"
#include "VulkanContext.hpp"
#include "../components/Mesh.hpp"
#include "vk/vulkan_defines.hpp"
#include "vk/resource/VertexBuffer.hpp"
// Debug
#include <debug/Logger.hpp>
// ASsetLoader
#include <assetloader/SceneGraph.hpp>
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
    [[nodiscard]] static std::unique_ptr<Impl> make_graphics(const odin::OdinInfo& info, window::Window& window)
    {
        // The circular dependencies for initialization is nuts..
        // So keep this as a stand alone function for clarity and then move everything into place

        // Make instance
        vk::Instance instance{ make_application_info(info.applicationName), window.required_extensions() };
        // Make surface
        vk::Surface surface{ window, instance.handle() };
        // Make physdev
        vk::PhysicalDevice physicalDevice{ instance, surface };
        // Make q fams
        vk::QueueFamilies queueFamilies{ physicalDevice, surface };
        // Make dev
        vk::Device device{ physicalDevice, queueFamilies };
        // Make Allocator
        auto pAllocator = std::make_shared<vk::Allocator>(instance, physicalDevice, device);
        // Make FrameHandler
        FrameHandler frameHandler{ device.handle(), queueFamilies.graphics(), queueFamilies.compute(), queueFamilies.transfer() };
        // Make FrameResources
        // Make Presenter
        Presenter presenter{ device, physicalDevice, std::move(surface), frameHandler };
        // Make Renderer
        Renderer renderer{ pAllocator, device.handle(), frameHandler };
        // Make TransferManager
        TransferManager transferManager{ device.handle(), queueFamilies.transfer() };
        // Make MeshRegisrty
        registry::mesh::MeshRegistry meshRegistry{ renderer.render_resources() };

        // Make Vulkan Context
        VulkanContext context{ std::move(instance),
                               std::move(physicalDevice),
                               std::move(queueFamilies),
                               std::move(device),
                               std::move(pAllocator) };
        // return Graphics as r value
        return std::make_unique<Impl>(std::move(context),
                                      std::move(frameHandler),
                                      std::move(presenter),
                                      std::move(renderer),
                                      std::move(transferManager),
                                      std::move(meshRegistry));
    }
public:
    Impl(VulkanContext context,
         FrameHandler frameHandler,
         Presenter presenter,
         Renderer renderer,
         TransferManager transferManager,
         registry::mesh::MeshRegistry meshRegistry)
        : m_Context{ std::move(context) }
        , m_FrameHandler{ std::move(frameHandler) }
        , m_Presenter{ std::move(presenter) }
        , m_Renderer{ std::move(renderer) }
        , m_TransferManager{ std::move(transferManager) }
        , m_MeshRegistry{ std::move(meshRegistry) }
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
        : m_Context{ std::move(other.m_Context) }
        , m_FrameHandler{ std::move(other.m_FrameHandler) }
        , m_Presenter{ std::move(other.m_Presenter) }
        , m_Renderer{ std::move(other.m_Renderer) }
        , m_TransferManager{ std::move(other.m_TransferManager) }
        , m_MeshRegistry{ std::move(other.m_MeshRegistry) }
    {}
    Impl& operator=(Impl&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            m_Context = std::move(other.m_Context);
            m_FrameHandler = std::move(other.m_FrameHandler);
            m_Presenter = std::move(other.m_Presenter);
            m_Renderer = std::move(other.m_Renderer);
            m_TransferManager = std::move(other.m_TransferManager);
            m_MeshRegistry = std::move(other.m_MeshRegistry);
        }
        return *this;
    }
public:
    void draw()
    {
        FrameContext frame = m_FrameHandler.start_frame();

        vk::CommandBuffer& transferBuffer = frame.transferBuffer.get();
        m_TransferManager.submit_transfer(transferBuffer);

        std::optional<ColorAttachment> colorAttach = m_Presenter.acquire_color_attachment(frame.colorAttachmentReady);
        if (colorAttach)
        {
            // Do rendering stuff
            vk::QueueView graphicsQ = m_Context.queue_families().graphics();
            m_Renderer.render_frame(colorAttach.value(), graphicsQ, frame, m_TransferManager);

            const vk::QueueFamilies& queues = m_Context.queue_families();
            if (!m_Presenter.present(queues.present(), frame.graphicsFinished))
            {
                LOG_WARN("Failed to present color attachment.");
            }
        }
    }
    void register_model(const asl::ModelHandle& handle)
    {
        vk::QueueView graphicsQ = m_Context.queue_families().graphics();
        const RenderResources& resources = m_Renderer.render_resources();
        std::shared_ptr<vk::Allocator> pAllocator = m_Context.allocator();
        m_MeshRegistry.register_model(m_TransferManager, resources, graphicsQ, pAllocator, handle);
    }
    // clang-format off
    bool is_registered(const asl::ModelHandle& handle) const 
    { 
        return m_MeshRegistry.contains(handle); 
    }
    // clang-format on
    std::vector<registry::mesh::MeshID> mesh_ids(const asl::ModelHandle& handle) const
    {
        // This is done to make sure MeshEntry does not leak across pimpl boundary
        // And to make sure ECS is not required in Graphics
        // There is probably a better way of doing this than copying the IDS..
        const std::vector<registry::mesh::MeshEntry>& meshEntries = m_MeshRegistry.entries(handle);
        std::vector<registry::mesh::MeshID> ids{};

        std::transform(std::begin(meshEntries),
                       std::end(meshEntries),
                       std::back_inserter(ids),
                       [](const registry::mesh::MeshEntry& entry) { return entry.id; });

        return ids;
    }
private:
    VulkanContext m_Context;
    FrameHandler m_FrameHandler;
    Presenter m_Presenter;
    Renderer m_Renderer;
    TransferManager m_TransferManager;
    registry::mesh::MeshRegistry m_MeshRegistry;
};
//
//
//
Graphics::Graphics(const OdinInfo& info, window::Window& window)
    : m_pImpl{ Impl::make_graphics(info, window) }
{}
Graphics::~Graphics() = default;
Graphics::Graphics(Graphics&& other) noexcept = default;
Graphics& Graphics::operator=(Graphics&& other) noexcept = default;
void Graphics::draw()
{
    m_pImpl->draw();
}
void Graphics::register_model(const asl::ModelHandle& handle)
{
    m_pImpl->register_model(handle);
}
bool Graphics::is_registered(const asl::ModelHandle& handle) const
{
    return m_pImpl->is_registered(handle);
}
std::vector<registry::mesh::MeshID> Graphics::mesh_ids(const asl::ModelHandle& handle) const
{
    return m_pImpl->mesh_ids(handle);
}
}    // namespace odin::graphics
