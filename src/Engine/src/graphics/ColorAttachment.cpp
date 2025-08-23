//
// Created by qwerty on 17/08/2025.
//
#include "ColorAttachment.hpp"
//
//
namespace odin::graphics
{
ColorAttachment::ColorAttachment(vk::Swapchain::AcquiredImage acquiredImage, VkExtent2D extent)
    : m_Image{ acquiredImage.image }
    , m_ImageView{ acquiredImage.view }
    , m_Index{ acquiredImage.index }
    , m_Extent{ extent }
{}
VkImageMemoryBarrier2 ColorAttachment::barrier_to_render() const
{
    static constexpr VkImageSubresourceRange subresourceRange{ .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                                               .baseMipLevel = 0,
                                                               .levelCount = 1,
                                                               .baseArrayLayer = 0,
                                                               .layerCount = 1 };
    return VkImageMemoryBarrier2{ .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                                  .pNext = nullptr,
                                  .srcStageMask = VK_PIPELINE_STAGE_2_NONE,
                                  .srcAccessMask = VK_ACCESS_2_NONE,
                                  .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                                  .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                                  .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                  .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                  .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                  .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                  .image = m_Image.handle,
                                  .subresourceRange = subresourceRange };
}
VkImageMemoryBarrier2 ColorAttachment::barrier_to_present() const
{
    static constexpr VkImageSubresourceRange subresourceRange{ .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                                               .baseMipLevel = 0,
                                                               .levelCount = 1,
                                                               .baseArrayLayer = 0,
                                                               .layerCount = 1 };
    return VkImageMemoryBarrier2{ .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                                  .pNext = nullptr,
                                  .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                                  .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                                  .dstStageMask = VK_PIPELINE_STAGE_2_NONE,
                                  .dstAccessMask = VK_ACCESS_2_NONE,
                                  .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                  .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                  .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                  .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                  .image = m_Image.handle,
                                  .subresourceRange = subresourceRange };
}
vk::resource::ImageViewRef ColorAttachment::view() const
{
    return m_ImageView;
}
VkExtent2D ColorAttachment::extent() const
{
    return m_Extent;
}
}    // namespace odin::graphics
