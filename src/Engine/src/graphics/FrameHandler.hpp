//
// Created by qwerty on 16/08/2025.
//
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
    vk::synchronization::SemaphoreRef colorAttachmentReady;
    vk::synchronization::SemaphoreRef graphicsFinished;
    std::reference_wrapper<vk::CommandBuffer> graphicsBuffer;
    std::reference_wrapper<vk::CommandBuffer> computeBuffer;
    std::reference_wrapper<vk::CommandBuffer> transferBuffer;
};
class FrameHandler
{
public:
    FrameHandler(vk::DeviceRef device, const vk::QueueView& graphics, const vk::QueueView& compute, const vk::QueueView& transfer);
    ~FrameHandler() = default;
    FrameHandler(const FrameHandler& other) = delete;
    FrameHandler(FrameHandler&& other) = default;
    FrameHandler& operator=(const FrameHandler& other) = delete;
    FrameHandler& operator=(FrameHandler&& other) = default;
public:
    [[nodiscard]] FrameContext start_frame();
    [[nodiscard]] std::uint32_t in_flight_count() const;
    [[nodiscard]] FrameIndex frame_index() const;
    void end_frame();
    void wait();
private:
    std::uint32_t m_NumInFlight;
    FrameIndex m_Frame;
    vk::DeviceRef m_Device;
    std::vector<vk::CommandPool> m_GraphicsPools;
    std::vector<vk::CommandBuffer> m_GraphicsBuffers;
    std::vector<vk::CommandPool> m_ComputePools;
    std::vector<vk::CommandBuffer> m_ComputeBuffers;
    std::vector<vk::CommandPool> m_TransferPools;
    std::vector<vk::CommandBuffer> m_TransferBuffers;
    std::vector<vk::synchronization::Semaphore> m_ColorAttachmentReady;
    std::vector<vk::synchronization::Semaphore> m_GraphicsFinished;
    std::vector<vk::synchronization::Fence> m_InFlight;    // Can be replaced with a single timeline semaphore
};
}    // namespace odin::graphics
