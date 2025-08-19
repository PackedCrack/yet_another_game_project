//
// Created by qwerty on 19/08/2025.
//
#pragma once

#include "vk/CommandBuffer.hpp"
#include "vk/Device.hpp"
#include "vk/QueueFamilies.hpp"
#include "vk/synchronization/TimelineSemaphore.hpp"
//
//
namespace odin::graphics
{
struct BufferTransfer
{
    vk::QueueView ownerQ;
    // std::unqiue_ptr<vk::resource::StagingBuffer> srcBuffer?
    VkBuffer srcBuffer;
    VkDeviceSize srcOffset;
    VkBuffer dstBuffer;
    VkDeviceSize dstOffset;
    VkDeviceSize size;
};
struct ImageTransfer
{};
struct TransferEpoch
{
    vk::synchronization::TimelineSemaphoreRef semaphore;
    std::uint64_t waitValue;
};
class TransferManager
{
public:
    TransferManager(vk::DeviceRef device, vk::QueueView transferQ);
public:
    void enqueue_buffer_transfer(BufferTransfer params);
    void enqueue_image_transfer(ImageTransfer params);
    [[nodiscard]] std::optional<TransferEpoch> submit_transfer(const vk::CommandBuffer& commandBuffer);
private:
    void acquire_buffers(vk::CommandBufferRef commandBuffer);
    void release_buffers(vk::CommandBufferRef commandBuffer);
    [[nodiscard]] bool record_buffer_transfers(vk::CommandBufferRef commandBuffer);
    [[nodiscard]] bool record_image_transfers(vk::CommandBufferRef commandBuffer);
    [[nodiscard]] std::vector<VkBufferMemoryBarrier2> make_batch_buffer_barrier_acquire();
    [[nodiscard]] std::vector<VkBufferMemoryBarrier2> make_batch_buffer_barrier_release();
private:
    vk::QueueView m_TransferQ;
    std::vector<BufferTransfer> m_BufferQueue;
    std::vector<ImageTransfer> m_ImageQueue;
    vk::synchronization::TimelineSemaphore m_Semaphore;
    std::uint64_t m_TransferID;
};
}    // namespace odin::graphics
