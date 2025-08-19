//
// Created by qwerty on 19/08/2025.
//
#include "TransferManager.hpp"
//
//
namespace
{
using namespace odin::graphics;
//
//
[[nodiscard]] VkBufferMemoryBarrier2
make_buffer_barrier(vk::QueueView currentOwner, vk::QueueView newOwner, VkBuffer dst, VkDeviceSize offset, VkDeviceSize size)
{
    VkBufferMemoryBarrier2 barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
    barrier.pNext = nullptr;
    barrier.srcQueueFamilyIndex = currentOwner.index;
    barrier.dstQueueFamilyIndex = newOwner.index;
    barrier.buffer = dst;
    barrier.offset = offset;
    barrier.size = size;

    return barrier;
}
[[nodiscard]] VkBufferMemoryBarrier2
make_buffer_barrier_acquire(vk::QueueView currentOwner, vk::QueueView newOwner, VkBuffer dst, VkDeviceSize offset, VkDeviceSize size)
{
    VkBufferMemoryBarrier2 barrier = make_buffer_barrier(currentOwner, newOwner, dst, offset, size);
    barrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
    barrier.srcAccessMask = VK_ACCESS_2_NONE;
    barrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    barrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;

    return barrier;
}
[[nodiscard]] VkBufferMemoryBarrier2
make_buffer_barrier_release(vk::QueueView currentOwner, vk::QueueView newOwner, VkBuffer dst, VkDeviceSize offset, VkDeviceSize size)
{
    VkBufferMemoryBarrier2 barrier = make_buffer_barrier(currentOwner, newOwner, dst, offset, size);
    barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    barrier.dstStageMask = VK_PIPELINE_STAGE_2_NONE;
    barrier.dstAccessMask = VK_ACCESS_2_NONE;

    return barrier;
}
}    // namespace
namespace odin::graphics
{
TransferManager::TransferManager(vk::DeviceRef device, vk::QueueView transferQ)
    : m_TransferQ{ transferQ }
    , m_BufferQueue{}
    , m_ImageQueue{}
    , m_Semaphore{ device }
    , m_TransferID{}
{}
void TransferManager::enqueue_buffer_transfer(BufferTransfer params)
{
    m_BufferQueue.push_back(params);
}
void TransferManager::enqueue_image_transfer(ImageTransfer params)
{
    m_ImageQueue.push_back(params);
}
std::optional<TransferEpoch> TransferManager::submit_transfer(const vk::CommandBuffer& commandBuffer)
{
    bool bufferCommands = record_buffer_transfers(commandBuffer.handle());
    bool imageCommands = record_image_transfers(commandBuffer.handle());
    if (!bufferCommands && !imageCommands)
    {
        return std::nullopt;
    }

    VkCommandBufferSubmitInfo cmdInfo = commandBuffer.submit_info();
    std::uint64_t signalValue = m_TransferID;
    VkSemaphoreSubmitInfo timelineInfo = m_Semaphore.submit_info(signalValue, VK_PIPELINE_STAGE_2_TRANSFER_BIT);

    VkSubmitInfo2 info2{ .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
                         .pNext = nullptr,
                         .flags = VK_NO_FLAGS,
                         .waitSemaphoreInfoCount = 0,
                         .pWaitSemaphoreInfos = nullptr,
                         .commandBufferInfoCount = 1,
                         .pCommandBufferInfos = std::addressof(cmdInfo),
                         .signalSemaphoreInfoCount = 1,
                         .pSignalSemaphoreInfos = std::addressof(timelineInfo) };
    vkQueueSubmit2(m_TransferQ.handle, 1, std::addressof(info2), VK_NULL_HANDLE);

    ++m_TransferID;
    m_BufferQueue.clear();
    m_ImageQueue.clear();

    return std::make_optional<TransferEpoch>(m_Semaphore.handle(), signalValue);
}
void TransferManager::acquire_buffers(vk::CommandBufferRef commandBuffer)
{
    std::vector<VkBufferMemoryBarrier2> acquireBatch = make_batch_buffer_barrier_acquire();

    VkDependencyInfo depAcq{ VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
    depAcq.bufferMemoryBarrierCount = static_cast<std::uint32_t>(acquireBatch.size());
    depAcq.pBufferMemoryBarriers = acquireBatch.data();

    vkCmdPipelineBarrier2(commandBuffer.handle, std::addressof(depAcq));
}
void TransferManager::release_buffers(vk::CommandBufferRef commandBuffer)
{
    std::vector<VkBufferMemoryBarrier2> releaseBatch = make_batch_buffer_barrier_release();

    VkDependencyInfo depRelease{ VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
    depRelease.bufferMemoryBarrierCount = static_cast<std::uint32_t>(releaseBatch.size());
    depRelease.pBufferMemoryBarriers = releaseBatch.data();

    vkCmdPipelineBarrier2(commandBuffer.handle, std::addressof(depRelease));
}
bool TransferManager::record_buffer_transfers(vk::CommandBufferRef commandBuffer)
{
    if (m_BufferQueue.empty())
    {
        return false;
    }

    acquire_buffers(commandBuffer);

    for (auto&& param : m_BufferQueue)
    {
        VkBufferCopy2 copy2{ .sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
                             .pNext = nullptr,
                             .srcOffset = param.srcOffset,
                             .dstOffset = param.dstOffset,
                             .size = param.size };
        VkCopyBufferInfo2 info2{ .sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
                                 .pNext = nullptr,
                                 .srcBuffer = param.srcBuffer,
                                 .dstBuffer = param.dstBuffer,
                                 .regionCount = 1,
                                 .pRegions = std::addressof(copy2) };
        vkCmdCopyBuffer2(commandBuffer.handle, std::addressof(info2));
    }

    release_buffers(commandBuffer);
    
    return true;
}
bool TransferManager::record_image_transfers(vk::CommandBufferRef commandBuffer)
{
    for (auto&& param : m_ImageQueue)
    {
        // submit some how
        VkImageCopy2 copy2{};
        VkCopyImageInfo2 info2{};
        vkCmdCopyImage2(commandBuffer.handle, std::addressof(info2));
    }

    return false;
}
std::vector<VkBufferMemoryBarrier2> TransferManager::make_batch_buffer_barrier_acquire()
{
    std::vector<VkBufferMemoryBarrier2> batch{};
    for (auto&& param : m_BufferQueue)
    {
        VkBufferMemoryBarrier2 barrier =
            make_buffer_barrier_acquire(param.ownerQ, m_TransferQ, param.dstBuffer, param.dstOffset, param.size);
        batch.push_back(barrier);
    }

    return batch;
}
std::vector<VkBufferMemoryBarrier2> TransferManager::make_batch_buffer_barrier_release()
{
    std::vector<VkBufferMemoryBarrier2> batch{};
    for (auto&& param : m_BufferQueue)
    {
        VkBufferMemoryBarrier2 barrier =
            make_buffer_barrier_release(m_TransferQ, param.ownerQ, param.dstBuffer, param.dstOffset, param.size);
        batch.push_back(barrier);
    }

    return batch;
}
}    // namespace odin::graphics
