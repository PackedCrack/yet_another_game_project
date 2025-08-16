#include "FrameHandler.hpp"

#include "vk/vulkan_defines.hpp"
//
//
namespace
{
using namespace odin::graphics;
[[nodiscard]] std::vector<vk::CommandPool> make_command_pools(std::uint32_t numFramesInFlight, vk::DeviceRef device, const vk::QueueView& queue)
{
    std::vector<vk::CommandPool> pools{};
    for (std::uint32_t i = 0; i < numFramesInFlight; ++i)
    {
        pools.emplace_back(device, queue);
    }

    return pools;
}
[[nodiscard]] std::vector<vk::CommandBuffer> make_command_buffers(const std::vector<vk::CommandPool>& pools)
{
    std::vector<vk::CommandBuffer> buffers{};
    for (auto&& pool : pools)
    {
        buffers.emplace_back(pool.make_command_buffer());
    }

    return buffers;
}
[[nodiscard]] std::vector<vk::synchronization::Semaphore> make_semaphores(std::uint32_t numFramesInFlight, vk::DeviceRef device)
{
    std::vector<vk::synchronization::Semaphore> semaphores{};
    for (std::uint32_t i = 0; i < numFramesInFlight; ++i)
    {
        semaphores.emplace_back(device);
    }
    return semaphores;
}
[[nodiscard]] std::vector<vk::synchronization::Fence> make_fences(std::uint32_t numFramesInFlight, vk::DeviceRef device)
{
    std::vector<vk::synchronization::Fence> fences{};
    for (std::uint32_t i = 0; i < numFramesInFlight; ++i)
    {
        fences.emplace_back(device);
    }
    return fences;
}
void wait_for_frame_in_flight(vk::DeviceRef device, vk::synchronization::FenceRef frameFence)
{
    static constexpr std::uint64_t timeout = UINT64_MAX;
    VK_CHECK(vkWaitForFences(device.handle, 1u, std::addressof(frameFence.handle), VK_TRUE, timeout),
             "Failed to wait for frame in flight Fence.");
    VK_CHECK(vkResetFences(device.handle, 1u, std::addressof(frameFence.handle)), 
             "Failed to reset frame in flight Fence.");
}
}   // namespace
namespace odin::graphics
{
FrameHandler::FrameHandler(vk::DeviceRef device, const vk::QueueView& graphics, const vk::QueueView& compute)
    : m_NumInFlight{ 2 }
    , m_Frame{ 0 }
    , m_Device{ device }
    , m_RenderPools{ make_command_pools(m_NumInFlight, device, graphics) }
    , m_RenderBuffers{ make_command_buffers(m_RenderPools) }
    , m_ComputePools{ make_command_pools(m_NumInFlight, device, compute) }
    , m_ComputeBuffers{ make_command_buffers(m_ComputePools) }
    , m_ColorAttachmentAvailable{ make_semaphores(m_NumInFlight, device) }
    , m_RenderingFinished{ make_semaphores(m_NumInFlight, device) }
    , m_InFlight{ make_fences(m_NumInFlight, device) }
{}
FrameContext FrameHandler::start_frame()
{
    FrameIndex index = frame_index();
    vk::synchronization::FenceRef frameFence = m_InFlight[index].handle();

    wait_for_frame_in_flight(m_Device, frameFence);
    ++m_Frame;

    return FrameContext{
        .frame = index,
        .inFlight = frameFence,
        .colorAttachmentAvailable = m_ColorAttachmentAvailable[index].handle(),
        .renderingFinished = m_RenderingFinished[index].handle()
    };
}
FrameIndex FrameHandler::frame_index() const
{
    return m_Frame % m_NumInFlight;
}
}    // namespace odin::graphics
