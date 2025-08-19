//
// Created by qwerty on 16/08/2025.
//
#include "FrameHandler.hpp"

#include "vk/vulkan_defines.hpp"
//
//
namespace
{
using namespace odin::graphics;
[[nodiscard]] std::vector<vk::CommandPool>
make_command_pools(std::uint32_t numFramesInFlight, vk::DeviceRef device, const vk::QueueView& queue)
{
    std::vector<vk::CommandPool> pools{};
    auto generator = [device, &queue]() { return vk::CommandPool{ device, queue }; };
    std::generate_n(std::back_inserter(pools), numFramesInFlight, generator);

    return pools;
}
[[nodiscard]] std::vector<vk::CommandBuffer> make_command_buffers(const std::vector<vk::CommandPool>& pools)
{
    std::vector<vk::CommandBuffer> buffers{};
    auto transform = [](const vk::CommandPool& pool) { return pool.make_command_buffer(); };
    std::transform(std::begin(pools), std::end(pools), std::back_inserter(buffers), transform);

    return buffers;
}
[[nodiscard]] std::vector<vk::synchronization::Semaphore> make_semaphores(std::uint32_t numFramesInFlight, vk::DeviceRef device)
{
    std::vector<vk::synchronization::Semaphore> semaphores{};
    auto generator = [device]() { return vk::synchronization::Semaphore{ device }; };
    std::generate_n(std::back_inserter(semaphores), numFramesInFlight, generator);

    return semaphores;
}
[[nodiscard]] std::vector<vk::synchronization::Fence> make_fences(std::uint32_t numFramesInFlight, vk::DeviceRef device)
{
    std::vector<vk::synchronization::Fence> fences{};
    auto generator = [device]() { return vk::synchronization::Fence{ device }; };
    std::generate_n(std::back_inserter(fences), numFramesInFlight, generator);

    return fences;
}
void wait_for_frame_in_flight(vk::DeviceRef device, vk::synchronization::FenceRef frameFence)
{
    static constexpr std::uint64_t timeout = UINT64_MAX;
    VK_CHECK(vkWaitForFences(device.handle, 1u, std::addressof(frameFence.handle), VK_TRUE, timeout),
             "Failed to wait for frame in flight Fence.");
    VK_CHECK(vkResetFences(device.handle, 1u, std::addressof(frameFence.handle)), "Failed to reset frame in flight Fence.");
}
}    // namespace
namespace odin::graphics
{
FrameHandler::FrameHandler(vk::DeviceRef device, const vk::QueueView& graphics, const vk::QueueView& compute)
    : m_NumInFlight{ 2 }
    , m_Frame{ 0 }
    , m_Device{ device }
    , m_GraphicsPools{ make_command_pools(m_NumInFlight, device, graphics) }
    , m_GraphicsBuffers{ make_command_buffers(m_GraphicsPools) }
    , m_ComputePools{ make_command_pools(m_NumInFlight, device, compute) }
    , m_ComputeBuffers{ make_command_buffers(m_ComputePools) }
    , m_TransferPools{ make_command_pools(m_NumInFlight, device, compute) }
    , m_TransferBuffers{ make_command_buffers(m_TransferPools) }
    , m_ColorAttachmentReady{ make_semaphores(m_NumInFlight, device) }
    , m_GraphicsFinished{ make_semaphores(m_NumInFlight, device) }
    , m_InFlight{ make_fences(m_NumInFlight, device) }
{}
FrameContext FrameHandler::start_frame()
{
    FrameIndex index = frame_index();
    vk::synchronization::FenceRef frameFence = m_InFlight[index].handle();

    wait_for_frame_in_flight(m_Device, frameFence);
    ++m_Frame;

    return FrameContext{ .frame = index,
                         .inFlight = frameFence,
                         .colorAttachmentReady = m_ColorAttachmentReady[index].handle(),
                         .graphicsFinished = m_GraphicsFinished[index].handle(),
                         .graphicsBuffer = m_GraphicsBuffers[index],
                         .computeBuffer = m_ComputeBuffers[index],
                         .transferBuffer = m_TransferBuffers[index] };
}
FrameIndex FrameHandler::frame_index() const
{
    return m_Frame % m_NumInFlight;
}
}    // namespace odin::graphics
