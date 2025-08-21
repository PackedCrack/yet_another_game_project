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
#include "MeshRegistry.hpp"

#include "vk/resource/VertexBuffer.hpp"
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
        // Make Allocator
        auto pAllocator = std::make_shared<vk::Allocator>(instance, physicalDevice, device);
        // Make FrameHandler
        FrameHandler frameHandler{ device.handle(), queueFamilies.graphics(), queueFamilies.compute(), queueFamilies.transfer() };
        // Make FrameResources
        // Make Presenter
        Presenter presenter{ device, physicalDevice, std::move(surface) };
        // Make Renderer
        Renderer renderer{ pAllocator };
        // Make TransferManager
        TransferManager transferManager{ device.handle(), queueFamilies.transfer() };
        // Make MeshRegisrty
        MeshRegistry meshRegistry{ renderer.render_resources() };

        // Make Vulkan Context
        VulkanContext context{ std::move(instance),
                               std::move(physicalDevice),
                               std::move(queueFamilies),
                               std::move(device),
                               std::move(pAllocator) };
        // return Graphics as r value
        return std::make_unique<Impl>(std::move(wnd),
                                      std::move(context),
                                      std::move(frameHandler),
                                      std::move(presenter),
                                      std::move(renderer),
                                      std::move(transferManager),
                                      std::move(meshRegistry));
    }
public:
    Impl(window::Window window,
         VulkanContext context,
         FrameHandler frameHandler,
         Presenter presenter,
         Renderer renderer,
         TransferManager transferManager,
         MeshRegistry meshRegistry)
        : m_Wnd{ std::move(window) }
        , m_Context{ std::move(context) }
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
        : m_Wnd{ std::move(other.m_Wnd) }
        , m_Context{ std::move(other.m_Context) }
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
            m_Wnd = std::move(other.m_Wnd);
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
        /*
        * std::vector<InstanceInfo> batches{};
        * registry.for_each<Mesh, TRS>{
        *   InstanceInfo i{};
        *   i.meshID = GeometryTracker.mesh_id(mesh.UUID)
        *   i.pos = TRS.translation
        *   i.orientation = TRS.orientation
        *   i.scale = TRS.scale
        * 
        *   batches.push_back(i)
        * }
        * Do instance counting here
        */
        ////
        ////
        using StagingBuffer = vk::resource::StagingBuffer;
        using VertexBuffer = vk::resource::VertexBuffer;
        using Vertex = vk::resource::Vertex;

        std::vector<Vertex> verticies(1024 * 1024);
        std::size_t numVerticies = verticies.size();
        VertexBuffer vertBuffer = m_Context.allocator()->create_vertex_buffer(numVerticies);

        BufferTransfer params{};
        params.ownerQ = m_Context.queue_families().graphics();
        params.dstBuffer = vertBuffer.handle();
        params.dstOffset = 0;
        params.size = verticies.size() * sizeof(Vertex);
        params.pSrcBuffer = std::make_unique<StagingBuffer>(m_Context.allocator()->create_staging_buffer(verticies.size(), sizeof(Vertex)));

        m_TransferManager.enqueue_buffer_transfer(std::move(params));
        ////
        ////


        // build batches and prepare
        // Camera pos
        // Direction light
        // TRS for entities


        FrameContext frame = m_FrameHandler.start_frame();


        vk::CommandBuffer& transferBuffer = frame.transferBuffer.get();
        m_TransferManager.submit_transfer(transferBuffer);

        std::optional<ColorAttachment> colorAttach = m_Presenter.acquire_color_attachment(frame.colorAttachmentReady);
        if (colorAttach)
        {
            // Do uploading
            // uploadermanager.upload()

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
    [[nodiscard]] MeshID mesh_id(const std::string& filepath /* Should be UUID*/)
    {
        if (!m_MeshRegistry.contains(filepath))
        {
            // Take AssetRegistry as parameter
            // asl::Model = assetRegistry.load(filepath);
            asl::Model lanternManyGroups{ R"(C:\Users\qwerty\Documents\repos\game\resources\assets\meshes\Lantern.glb)" };

            vk::QueueView graphicsQ = m_Context.queue_families().graphics();
            m_MeshRegistry.load_model(m_TransferManager, graphicsQ, m_Context.allocator(), lanternManyGroups);
        }

        return m_MeshRegistry.get_mesh(filepath);
    }
private:
    window::Window m_Wnd;
    VulkanContext m_Context;
    FrameHandler m_FrameHandler;
    Presenter m_Presenter;
    Renderer m_Renderer;
    TransferManager m_TransferManager;
    MeshRegistry m_MeshRegistry;
};
//
//
//
Graphics::Graphics(const OdinInfo& info)
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
