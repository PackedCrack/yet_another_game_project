#include "Renderer.hpp"

#include "vk/vulkan_defines.hpp"
//
//
namespace
{
using QueueView = odin::graphics::vk::QueueView;
using CommandBuffer = odin::graphics::vk::CommandBuffer;
using CommandBufferRef = odin::graphics::vk::CommandBufferRef;
using FenceRef = odin::graphics::vk::synchronization::FenceRef;
using SemaphoreRef = odin::graphics::vk::synchronization::SemaphoreRef;
//
//
VkSemaphoreSubmitInfo submit_info_semaphore(VkSemaphore semaphore, std::uint64_t value, VkPipelineStageFlags2 flags)
{
    return VkSemaphoreSubmitInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .pNext = nullptr,
        .semaphore = semaphore,
        .value = value,
        .stageMask = flags,

    };
}
template<typename... stage_mask_t>
requires(std::same_as<VkPipelineStageFlags2, std::remove_cvref_t<stage_mask_t>> && ...)
[[nodiscard]] VkSemaphoreSubmitInfo
submit_info_timeline_semaphore(SemaphoreRef semaphore, std::uint64_t value, VkPipelineStageFlags2 mask, stage_mask_t&&... masks)
{
    VkPipelineStageFlags2 flags = (mask | ... | masks);
    return submit_info_semaphore(semaphore.handle, value, flags);
}
template<typename... stage_mask_t>
requires(std::same_as<VkPipelineStageFlags2, std::remove_cvref_t<stage_mask_t>> && ...)
[[nodiscard]] VkSemaphoreSubmitInfo
submit_info_binary_semaphore(SemaphoreRef semaphore, VkPipelineStageFlags2 mask, stage_mask_t&&... masks)
{
    VkPipelineStageFlags2 flags = (mask | ... | masks);
    return submit_info_semaphore(semaphore.handle, 0, flags);
}
VkCommandBufferSubmitInfo submit_info_cmd_buffer(CommandBufferRef cmdBuffer)
{
    return VkCommandBufferSubmitInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
                                      .pNext = nullptr,
                                      .commandBuffer = cmdBuffer.handle,
                                      .deviceMask = 0 };
}
void submit(QueueView queue,
            const CommandBuffer& cmdBuffer,
            SemaphoreRef colorAttachmentReady,
            SemaphoreRef graphicsFinished,
            FenceRef inFlight)
{
    CommandBufferRef cb = cmdBuffer.handle();

    VkCommandBufferSubmitInfo cbInfo = submit_info_cmd_buffer(cb);
    VkSemaphoreSubmitInfo colorAttachReadyInfo =
        submit_info_binary_semaphore(colorAttachmentReady, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
    VkSemaphoreSubmitInfo graphicsFinishedInfo = submit_info_binary_semaphore(graphicsFinished, VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT);

    VkSubmitInfo2 submitInfo = { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
                                 .pNext = nullptr,
                                 .flags = VK_NO_FLAGS,
                                 .waitSemaphoreInfoCount = 1,
                                 .pWaitSemaphoreInfos = std::addressof(colorAttachReadyInfo),
                                 .commandBufferInfoCount = 1,
                                 .pCommandBufferInfos = std::addressof(cbInfo),
                                 .signalSemaphoreInfoCount = 1,
                                 .pSignalSemaphoreInfos = std::addressof(graphicsFinishedInfo) };

    VK_CHECK(vkQueueSubmit2(queue.handle, 1, std::addressof(submitInfo), inFlight.handle), "Failed to submit to Graphics Queue!");
}
}    // namespace
namespace odin::graphics
{
void Renderer::render_frame(vk::QueueView graphics, const FrameContext& frameContext)
{
    vk::CommandBuffer& gfxCmdBuffer = frameContext.graphicsBuffer.get();
    // Reset cmdBuffer and prepare it for commands
    gfxCmdBuffer.reset();
    gfxCmdBuffer.begin();

    // Bind Global buffers

    // Bind descriptors

    // Bind pipelines
    //m_Pipeline.bind(cmdBuffer.handle());


    // Dynamic rendering


    // End rendering
    gfxCmdBuffer.end();


    // Submit
    submit(graphics, gfxCmdBuffer, frameContext.colorAttachmentReady, frameContext.graphicsFinished, frameContext.inFlight);
}
}    // namespace odin::graphics
