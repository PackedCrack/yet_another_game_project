#pragma once

#include "vk/CommandPool.hpp"
#include "vk/Device.hpp"
#include "vk/PhysicalDevice.hpp"
#include "vk/resource/ImageView.hpp"
#include "vk/synchronization/Fence.hpp"
#include "vk/synchronization/Semaphore.hpp"
//
//
namespace odin::graphics
{
using FrameIndex = std::uint32_t;
struct FrameContext
{
    FrameIndex frame;
    vk::synchronization::FenceRef inFlight;
    vk::synchronization::SemaphoreRef colorAttachmentAvailable;
    vk::synchronization::SemaphoreRef renderingFinished;
};
class FrameHandler
{
public:
    FrameHandler(vk::DeviceRef device, const vk::QueueView& graphics, const vk::QueueView& compute);
    ~FrameHandler() = default;
    FrameHandler(const FrameHandler& other) = delete;
    FrameHandler(FrameHandler&& other) = default;
    FrameHandler& operator=(const FrameHandler& other) = delete;
    FrameHandler& operator=(FrameHandler&& other) = default;
public:
    [[nodiscard]] FrameContext start_frame();
private:
    [[nodiscard]] FrameIndex frame_index() const;
private:
    std::uint32_t m_NumInFlight;
    FrameIndex m_Frame;
    vk::DeviceRef m_Device;
    std::vector<vk::CommandPool> m_RenderPools;
    std::vector<vk::CommandBuffer> m_RenderBuffers;
    std::vector<vk::CommandPool> m_ComputePools;
    std::vector<vk::CommandBuffer> m_ComputeBuffers;
    std::vector<vk::synchronization::Semaphore> m_ColorAttachmentAvailable;
    std::vector<vk::synchronization::Semaphore> m_RenderingFinished;
    std::vector<vk::synchronization::Fence> m_InFlight;
};
}    // namespace odin::graphics
