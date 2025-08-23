//
// Created by qwerty on 19/08/2025.
//
#include "TransferManager.hpp"
//
//
// NOTES ABOUT TRANSFERS:
// If an application does not need the contents of a resource to remain valid when transferring
// from one queue family to another, then the ownership transfer should be skipped.
// https://stackoverflow.com/questions/60310004/do-i-need-to-transfer-ownership-back-to-the-transfer-queue-on-next-transfer
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
    , m_CurrentEpoch{ std::nullopt }
{}
void TransferManager::enqueue_buffer_transfer(vk::resource::BufferRef dst,
                                              std::unique_ptr<vk::resource::StagingBuffer> pStagingBuffer,
                                              const vk::QueueView& graphicsQ,
                                              const ArenaAllocation& allocation)
{
    BufferTransfer params{};
    params.ownerQ = graphicsQ;
    params.dstBuffer = dst;
    params.dstOffset = allocation.start();
    params.size = allocation.size();
    params.pSrcBuffer = std::move(pStagingBuffer);

    enqueue_buffer_transfer(std::move(params));
}
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
void TransferManager::record_buffer_acquisition(vk::QueueView newOwner, vk::CommandBufferRef commandBuffer) const
{
    const std::vector<BufferTransfer>& transfers = buffer_transfers();
    if (transfers.empty())
    {
        return;
    }

    std::vector<VkBufferMemoryBarrier2> acquireBatch = make_batch_buffer_barrier_acquire(newOwner, transfers);

    VkDependencyInfo depAcq{ VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
    depAcq.bufferMemoryBarrierCount = static_cast<std::uint32_t>(acquireBatch.size());
    depAcq.pBufferMemoryBarriers = acquireBatch.data();

    vkCmdPipelineBarrier2(commandBuffer.handle, std::addressof(depAcq));
}
void TransferManager::submit_transfer(vk::CommandBuffer& commandBuffer)
{
    cycle_transfer_lists();

    commandBuffer.reset();
    commandBuffer.begin();

    vk::CommandBufferRef cmdBuf = commandBuffer.handle();
    if (record_buffer_transfers(cmdBuf) || record_image_transfers(cmdBuf))
    {
        commandBuffer.end();

        VkCommandBufferSubmitInfo cmdInfo = commandBuffer.submit_info();
        std::uint64_t signalValue = m_TransferID;
        VkSemaphoreSubmitInfo timelineInfo = m_Semaphore.submit_info(signalValue, VK_PIPELINE_STAGE_2_TRANSFER_BIT);

        VkSubmitInfo2 info2{};
        info2.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
        info2.pNext = nullptr;
        info2.flags = VK_NO_FLAGS;
        info2.waitSemaphoreInfoCount = 0;
        info2.pWaitSemaphoreInfos = nullptr;
        info2.commandBufferInfoCount = 1;
        info2.pCommandBufferInfos = std::addressof(cmdInfo);
        info2.signalSemaphoreInfoCount = 1;
        info2.pSignalSemaphoreInfos = std::addressof(timelineInfo);

        vkQueueSubmit2(m_TransferQ.handle, 1, std::addressof(info2), VK_NULL_HANDLE);

        ++m_TransferID;
        m_CurrentEpoch = std::make_optional<TransferEpoch>(m_Semaphore.handle(), signalValue);
    }
    else
    {
        m_CurrentEpoch = std::nullopt;
    }
}
std::optional<TransferEpoch> TransferManager::epoch() const
{
    return m_CurrentEpoch;
}
void TransferManager::cycle_transfer_lists()
{
    m_BufferQueue.splice(std::end(m_BufferQueue), m_BufferQueue, std::begin(m_BufferQueue));
    m_BufferQueue.front().clear();
    m_ImageQueue.splice(std::end(m_ImageQueue), m_ImageQueue, std::begin(m_ImageQueue));
    m_ImageQueue.front().clear();
}
void TransferManager::record_buffer_releases(vk::CommandBufferRef commandBuffer, const std::vector<BufferTransfer>& transfers)
{
    std::vector<VkBufferMemoryBarrier2> releaseBatch = make_batch_buffer_barrier_release(transfers);

    VkDependencyInfo depRelease{ VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
    depRelease.bufferMemoryBarrierCount = static_cast<std::uint32_t>(releaseBatch.size());
    depRelease.pBufferMemoryBarriers = releaseBatch.data();

    vkCmdPipelineBarrier2(commandBuffer.handle, std::addressof(depRelease));
}
bool TransferManager::record_buffer_transfers(vk::CommandBufferRef commandBuffer)
{
    const std::vector<BufferTransfer>& transfers = buffer_transfers();
    if (transfers.empty())
    {
        return false;
    }

    for (auto&& param : transfers)
    {
        VkBufferCopy2 copy2{};
        copy2.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2;
        copy2.pNext = nullptr;
        copy2.srcOffset = 0;
        copy2.dstOffset = param.dstOffset;
        copy2.size = param.size;

        vk::resource::BufferRef srcBuffer = param.pSrcBuffer->handle();
        VkCopyBufferInfo2 info2{};
        info2.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2;
        info2.pNext = nullptr;
        info2.srcBuffer = srcBuffer.handle;
        info2.dstBuffer = param.dstBuffer.handle;
        info2.regionCount = 1;
        info2.pRegions = std::addressof(copy2);

        vkCmdCopyBuffer2(commandBuffer.handle, std::addressof(info2));
    }

    record_buffer_releases(commandBuffer, transfers);

    return true;
}
bool TransferManager::record_image_transfers(vk::CommandBufferRef commandBuffer)
{
    const std::vector<ImageTransfer>& transfers = image_transfers();
    if (transfers.empty())
    {
        return false;
    }

    for (auto&& param : transfers)
    {
        // submit some how
        VkImageCopy2 copy2{};
        VkCopyImageInfo2 info2{};
        vkCmdCopyImage2(commandBuffer.handle, std::addressof(info2));
    }

    return true;
}
std::vector<VkBufferMemoryBarrier2> TransferManager::make_batch_buffer_barrier_acquire(vk::QueueView newOwner,
                                                                                       const std::vector<BufferTransfer>& transfers) const
{
    std::vector<VkBufferMemoryBarrier2> batch{};
    for (auto&& param : transfers)
    {
        VkBuffer dstBuffer = param.dstBuffer.handle;
        VkDeviceSize offset = param.dstOffset;
        VkDeviceSize size = param.size;

        VkBufferMemoryBarrier2 barrier = make_buffer_barrier_acquire(param.ownerQ, newOwner, dstBuffer, offset, size);
        batch.push_back(barrier);
    }

    return batch;
}
std::vector<VkBufferMemoryBarrier2> TransferManager::make_batch_buffer_barrier_release(const std::vector<BufferTransfer>& transfers) const
{
    std::vector<VkBufferMemoryBarrier2> batch{};
    for (auto&& param : transfers)
    {
        VkBuffer dstBuffer = param.dstBuffer.handle;
        VkDeviceSize offset = param.dstOffset;
        VkDeviceSize size = param.size;

        VkBufferMemoryBarrier2 barrier = make_buffer_barrier_release(m_TransferQ, param.ownerQ, dstBuffer, offset, size);
        batch.push_back(barrier);
    }

    return batch;
}
const TransferManager::BufferTransfers& TransferManager::buffer_transfers() const
{
    return m_BufferQueue.back();
}
const TransferManager::ImageTransfers& TransferManager::image_transfers() const
{
    return m_ImageQueue.back();
}
}    // namespace odin::graphics
