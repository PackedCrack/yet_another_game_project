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
    , m_BufferQueue{ 4 }
    , m_ImageQueue{ 4 }
    , m_Semaphore{ device }
    , m_TransferID{ 1 }
{}
void TransferManager::enqueue_buffer_transfer(BufferTransfer params)
{
    std::vector<BufferTransfer>& q = m_BufferQueue.front();
    q.push_back(std::move(params));
}
void TransferManager::enqueue_image_transfer(ImageTransfer params)
{
    std::vector<ImageTransfer>& q = m_ImageQueue.front();
    q.push_back(params);
}
std::optional<TransferEpoch> TransferManager::submit_transfer(vk::CommandBuffer& commandBuffer)
{
    auto [bufferTransfers, imageTransfers] = cycle_transfer_lists();

    commandBuffer.reset();
    commandBuffer.begin();

    bool bufferCommands = record_buffer_transfers(commandBuffer.handle(), bufferTransfers);
    bool imageCommands = record_image_transfers(commandBuffer.handle(), imageTransfers);
    if (!bufferCommands && !imageCommands)
    {
        return std::nullopt;
    }

    commandBuffer.end();

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

    return std::make_optional<TransferEpoch>(m_Semaphore.handle(), signalValue);
}
std::tuple<TransferManager::BufferTransfers&, TransferManager::ImageTransfers&> TransferManager::cycle_transfer_lists()
{
    m_BufferQueue.splice(std::end(m_BufferQueue), m_BufferQueue, std::begin(m_BufferQueue));
    m_BufferQueue.front().clear();
    m_ImageQueue.splice(std::end(m_ImageQueue), m_ImageQueue, std::begin(m_ImageQueue));
    m_ImageQueue.front().clear();

    return { m_BufferQueue.back(), m_ImageQueue.back() };
}
void TransferManager::acquire_buffers(vk::CommandBufferRef commandBuffer, const std::vector<BufferTransfer>& transfers)
{
    std::vector<VkBufferMemoryBarrier2> acquireBatch = make_batch_buffer_barrier_acquire(transfers);

    VkDependencyInfo depAcq{ VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
    depAcq.bufferMemoryBarrierCount = static_cast<std::uint32_t>(acquireBatch.size());
    depAcq.pBufferMemoryBarriers = acquireBatch.data();

    vkCmdPipelineBarrier2(commandBuffer.handle, std::addressof(depAcq));
}
void TransferManager::release_buffers(vk::CommandBufferRef commandBuffer, const std::vector<BufferTransfer>& transfers)
{
    std::vector<VkBufferMemoryBarrier2> releaseBatch = make_batch_buffer_barrier_release(transfers);

    VkDependencyInfo depRelease{ VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
    depRelease.bufferMemoryBarrierCount = static_cast<std::uint32_t>(releaseBatch.size());
    depRelease.pBufferMemoryBarriers = releaseBatch.data();

    vkCmdPipelineBarrier2(commandBuffer.handle, std::addressof(depRelease));
}
bool TransferManager::record_buffer_transfers(vk::CommandBufferRef commandBuffer, const std::vector<BufferTransfer>& transfers)
{
    if (transfers.empty())
    {
        return false;
    }

    acquire_buffers(commandBuffer, transfers);

    for (auto&& param : transfers)
    {
        VkBufferCopy2 copy2{ .sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
                             .pNext = nullptr,
                             .srcOffset = 0,
                             .dstOffset = param.dstOffset,
                             .size = param.size };

        vk::resource::BufferRef srcBuffer = param.pSrcBuffer->handle();
        VkCopyBufferInfo2 info2{ .sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
                                 .pNext = nullptr,
                                 .srcBuffer = srcBuffer.handle,
                                 .dstBuffer = param.dstBuffer.handle,
                                 .regionCount = 1,
                                 .pRegions = std::addressof(copy2) };
        vkCmdCopyBuffer2(commandBuffer.handle, std::addressof(info2));
    }

    release_buffers(commandBuffer, transfers);

    return true;
}
bool TransferManager::record_image_transfers(vk::CommandBufferRef commandBuffer, const std::vector<ImageTransfer>& transfers)
{
    for (auto&& param : transfers)
    {
        // submit some how
        VkImageCopy2 copy2{};
        VkCopyImageInfo2 info2{};
        vkCmdCopyImage2(commandBuffer.handle, std::addressof(info2));
    }

    return false;
}
std::vector<VkBufferMemoryBarrier2> TransferManager::make_batch_buffer_barrier_acquire(const std::vector<BufferTransfer>& transfers) const
{
    std::vector<VkBufferMemoryBarrier2> batch{};
    for (auto&& param : transfers)
    {
        VkBufferMemoryBarrier2 barrier =
            make_buffer_barrier_acquire(param.ownerQ, m_TransferQ, param.dstBuffer.handle, param.dstOffset, param.size);
        batch.push_back(barrier);
    }

    return batch;
}
std::vector<VkBufferMemoryBarrier2> TransferManager::make_batch_buffer_barrier_release(const std::vector<BufferTransfer>& transfers) const
{
    std::vector<VkBufferMemoryBarrier2> batch{};
    for (auto&& param : transfers)
    {
        VkBufferMemoryBarrier2 barrier =
            make_buffer_barrier_release(m_TransferQ, param.ownerQ, param.dstBuffer.handle, param.dstOffset, param.size);
        batch.push_back(barrier);
    }

    return batch;
}
}    // namespace odin::graphics
