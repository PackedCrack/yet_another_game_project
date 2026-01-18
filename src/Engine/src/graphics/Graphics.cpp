//
// Created by qwerty on 11/08/2025.
//
#include "Graphics.hpp"

#include "FrameHandler.hpp"
#include "Presenter.hpp"
#include "Renderer.hpp"
#include "TransferManager.hpp"
#include "VulkanContext.hpp"
#include "registry/mesh/MeshRegistry.hpp"
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
// TODO: I pulled these numbers from my ass
static constexpr std::int32_t MAX_INSTANCES = 41'94304;
static constexpr std::int32_t MAX_DRAWS = 2048;
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
        using ResourceRegistry = registry::resource::ResourceRegistry;
        using PipelineRegistry = registry::pipeline::PipelineRegistry;

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
        // Make Presenter
        Presenter presenter{ device, physicalDevice, std::move(surface), frameHandler };
        // Make ResourceRegistry
        ResourceRegistry resourceRegistry{ device.handle(), pAllocator, frameHandler, MAX_DRAWS, MAX_INSTANCES };
        // Make PipelineRegistry
        auto pPipelineRegistry = PipelineRegistry::make(device.handle());
        // Make Renderer
        Renderer renderer{ device.handle(), presenter, resourceRegistry, *pPipelineRegistry };
        // Make TransferManager
        TransferManager transferManager{ device.handle(), queueFamilies.transfer() };
        // Make MeshRegisrty
        registry::mesh::MeshRegistry meshRegistry{ resourceRegistry };


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
                                      std::move(meshRegistry),
                                      std::move(resourceRegistry),
                                      std::move(pPipelineRegistry));
    }
public:
    Impl(VulkanContext context,
         FrameHandler frameHandler,
         Presenter presenter,
         Renderer renderer,
         TransferManager transferManager,
         registry::mesh::MeshRegistry meshRegistry,
         registry::resource::ResourceRegistry resourceRegistry,
         std::unique_ptr<registry::pipeline::PipelineRegistry> pPipelineRegistry)
        : m_Context{ std::move(context) }
        , m_FrameHandler{ std::move(frameHandler) }
        , m_Presenter{ std::move(presenter) }
        , m_Renderer{ std::move(renderer) }
        , m_TransferManager{ std::move(transferManager) }
        , m_MeshRegistry{ std::move(meshRegistry) }
        , m_ResourceRegistry{ std::move(resourceRegistry) }
        , m_pPipelineRegistry{ std::move(pPipelineRegistry) }
    {}
    ~Impl()
    {
        // Force wait for GPU when exiting the application..
        VkDevice device = m_Context.device().handle().handle;
        if (device != VK_NULL_HANDLE)
        {
            if (vkDeviceWaitIdle(device) != VK_SUCCESS)
            {
                LOG_ERR("Failed to await for GPU to idle when exiting.");
            }
        }
    }
    Impl(Impl&& other) noexcept
        : m_Context{ std::move(other.m_Context) }
        , m_FrameHandler{ std::move(other.m_FrameHandler) }
        , m_Presenter{ std::move(other.m_Presenter) }
        , m_Renderer{ std::move(other.m_Renderer) }
        , m_TransferManager{ std::move(other.m_TransferManager) }
        , m_MeshRegistry{ std::move(other.m_MeshRegistry) }
        , m_ResourceRegistry{ std::move(other.m_ResourceRegistry) }
        , m_pPipelineRegistry{ std::move(other.m_pPipelineRegistry) }
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
            m_ResourceRegistry = std::move(other.m_ResourceRegistry);
            m_pPipelineRegistry = std::move(other.m_pPipelineRegistry);
        }
        return *this;
    }
public:
    void draw(std::span<const InstanceInfo> instanceInfos)
    {
        FrameContext frame = m_FrameHandler.start_frame();

        std::int32_t instanceCount = enqueue_instance_infos(instanceInfos, frame);


        m_FrameHandler.wait();

        vk::CommandBuffer& cmdBuf = frame.transferBuffer.get();
        m_TransferManager.submit_transfers(cmdBuf);

        std::optional<ColorAttachment> colorAttach = m_Presenter.acquire_color_attachment(frame.colorAttachmentReady);
        if (colorAttach)
        {
            // Do rendering stuff
            const ColorAttachment& ca = colorAttach.value();
            vk::QueueView graphicsQ = m_Context.queue_families().graphics();
            m_Renderer.render_frame(ca, graphicsQ, frame, m_TransferManager, m_ResourceRegistry, instanceCount);

            const vk::QueueFamilies& queues = m_Context.queue_families();
            if (!m_Presenter.present(queues.present(), frame.graphicsFinished))
            {
                LOG_WARN("Failed to present color attachment.");
            }
        }

        m_FrameHandler.end_frame();
    }
    std::int32_t enqueue_instance_infos(std::span<const InstanceInfo> instanceInfos, const FrameContext& frame)
    {
        using ResourceRegistry = registry::resource::ResourceRegistry;
        using StagingBuffer = vk::resource::StagingBuffer;

        BufferTransfer transfer{};

        vk::QueueView graphicsQ = m_Context.queue_families().graphics();
        transfer.ownerQ = graphicsQ;
        transfer.pSrcBuffer = std::make_unique<StagingBuffer>(m_Context.allocator()->to_staging_buffer(instanceInfos));

        auto instanceInfo = m_ResourceRegistry.dynamic_storage_buffer(ResourceRegistry::DYN_SSBO_INSTANCE_INFO);
        transfer.dstBuffer = instanceInfo->handle();
        transfer.dstOffset = instanceInfo->offset(frame.frame);
        transfer.size = instanceInfos.size() * sizeof(decltype(instanceInfos)::value_type);

        m_TransferManager.enqueue_buffer_transfer(std::move(transfer));

        return static_cast<std::int32_t>(instanceInfos.size());
    }
    std::vector<std::int32_t> register_model(const asl::ModelHandle& handle)
    {
        vk::QueueView graphicsQ = m_Context.queue_families().graphics();
        std::shared_ptr<vk::Allocator> pAllocator = m_Context.allocator();
        const std::vector<registry::mesh::MeshEntry>& meshEntries =
            m_MeshRegistry.register_model(m_TransferManager, m_ResourceRegistry, graphicsQ, pAllocator, handle);


        // This is done to make sure MeshEntry does not leak across pimpl boundary
        // And to make sure ECS is not required in Graphics
        // There is probably a better way of doing this than copying the IDS..
        std::vector<std::int32_t> ids{};
        std::transform(std::begin(meshEntries),
                       std::end(meshEntries),
                       std::back_inserter(ids),
                       [](const registry::mesh::MeshEntry& entry)
                       {
                           static_assert(std::convertible_to<registry::mesh::MeshID, std::int32_t>);
                           return static_cast<std::int32_t>(entry.id);
                       });

        return ids;
    }
    // clang-format off
    bool is_registered(const asl::ModelHandle& handle) const 
    { 
        return m_MeshRegistry.contains(handle); 
    }
    // clang-format on
private:
    VulkanContext m_Context;
    FrameHandler m_FrameHandler;
    Presenter m_Presenter;
    Renderer m_Renderer;
    TransferManager m_TransferManager;
    registry::mesh::MeshRegistry m_MeshRegistry;
    registry::resource::ResourceRegistry m_ResourceRegistry;
    std::unique_ptr<registry::pipeline::PipelineRegistry> m_pPipelineRegistry;
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
void Graphics::draw(std::span<const InstanceInfo> instanceInfos)
{
    m_pImpl->draw(instanceInfos);
}
std::vector<std::int32_t> Graphics::register_model(const asl::ModelHandle& handle)
{
    return m_pImpl->register_model(handle);
}
bool Graphics::is_registered(const asl::ModelHandle& handle) const
{
    return m_pImpl->is_registered(handle);
}
}    // namespace odin::graphics
