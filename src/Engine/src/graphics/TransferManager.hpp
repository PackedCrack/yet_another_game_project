//
// Created by qwerty on 19/08/2025.
//
#pragma once

#include "../ArenaAllocator.hpp"
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
    void enqueue_buffer_transfer(vk::resource::BufferRef dst,
                                 std::unique_ptr<vk::resource::StagingBuffer> pStagingBuffer,
                                 const vk::QueueView& graphicsQ,
                                 const ArenaAllocation& allocation);
    void enqueue_buffer_transfer(BufferTransfer params);
    void enqueue_image_transfer(ImageTransfer params);
    void record_buffer_acquisition(vk::QueueView newOwner, vk::CommandBufferRef commandBuffer) const;
    void submit_transfer(vk::CommandBuffer& commandBuffer);
    [[nodiscard]] std::optional<TransferEpoch> epoch() const;
private:
    void record_buffer_releases(vk::CommandBufferRef commandBuffer, const std::vector<BufferTransfer>& transfers);
    void cycle_transfer_lists();
    [[nodiscard]] bool record_buffer_transfers(vk::CommandBufferRef commandBuffer);
    [[nodiscard]] bool record_image_transfers(vk::CommandBufferRef commandBuffer);
    [[nodiscard]] std::vector<VkBufferMemoryBarrier2> make_batch_buffer_barrier_acquire(vk::QueueView newOwner,
                                                                                        const std::vector<BufferTransfer>& transfers) const;
    [[nodiscard]] std::vector<VkBufferMemoryBarrier2> make_batch_buffer_barrier_release(const std::vector<BufferTransfer>& transfers) const;
    [[nodiscard]] const BufferTransfers& buffer_transfers() const;
    [[nodiscard]] const ImageTransfers& image_transfers() const;
private:
    vk::QueueView m_TransferQ;
    BufferTransferQueue m_BufferQueue;
    ImageTransfersQueue m_ImageQueue;
    vk::synchronization::TimelineSemaphore m_Semaphore;
    std::uint64_t m_TransferID;
    std::optional<TransferEpoch> m_CurrentEpoch;
};
}    // namespace odin::graphics
