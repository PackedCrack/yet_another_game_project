//
// Created by qwerty on 19/08/2025.
//
#pragma once

#include "vk/CommandBuffer.hpp"
#include "vk/Device.hpp"
#include "vk/QueueFamilies.hpp"
#include "vk/resource/StagingBuffer.hpp"
#include "vk/synchronization/TimelineSemaphore.hpp"
//
//
namespace odin::graphics
{
struct BufferTransfer
{
    vk::QueueView ownerQ;
    std::unique_ptr<vk::resource::StagingBuffer> pSrcBuffer;
    vk::resource::BufferRef dstBuffer;
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
    using BufferTransfers = std::vector<BufferTransfer>;
    using ImageTransfers = std::vector<ImageTransfer>;
    using BufferTransferQueue = std::list<std::vector<BufferTransfer>>;
    using ImageTransfersQueue = std::list<std::vector<ImageTransfer>>;
public:
    TransferManager(vk::DeviceRef device, vk::QueueView transferQ);
public:
    void enqueue_buffer_transfer(BufferTransfer params);
    void enqueue_image_transfer(ImageTransfer params);
    [[nodiscard]] std::optional<TransferEpoch> submit_transfer(vk::CommandBuffer& commandBuffer);
private:
    void acquire_buffers(vk::CommandBufferRef commandBuffer, const std::vector<BufferTransfer>& transfers);
    void release_buffers(vk::CommandBufferRef commandBuffer, const std::vector<BufferTransfer>& transfers);
    [[nodiscard]] std::tuple<BufferTransfers&, ImageTransfers&> cycle_transfer_lists();
    [[nodiscard]] bool record_buffer_transfers(vk::CommandBufferRef commandBuffer, const std::vector<BufferTransfer>& transfers);
    [[nodiscard]] bool record_image_transfers(vk::CommandBufferRef commandBuffer, const std::vector<ImageTransfer>& transfers);
    [[nodiscard]] std::vector<VkBufferMemoryBarrier2> make_batch_buffer_barrier_acquire(const std::vector<BufferTransfer>& transfers) const;
    [[nodiscard]] std::vector<VkBufferMemoryBarrier2> make_batch_buffer_barrier_release(const std::vector<BufferTransfer>& transfers) const;
private:
    vk::QueueView m_TransferQ;
    BufferTransferQueue m_BufferQueue;
    ImageTransfersQueue m_ImageQueue;
    vk::synchronization::TimelineSemaphore m_Semaphore;
    std::uint64_t m_TransferID;
};
}    // namespace odin::graphics
