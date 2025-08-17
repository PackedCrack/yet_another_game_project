//
// Created by qwerty on 17/08/2025.
//
#pragma once

#include "vk/Swapchain.hpp"
//
//
namespace odin::graphics
{
class ColorAttachment
{
    using swapchain_index_t = vk::Swapchain::index_t;
public:
    ColorAttachment(vk::Swapchain::AcquiredImage acquiredImage, VkExtent2D extent);
public:
    [[nodiscard]] VkImageMemoryBarrier2 barrier_to_render() const;
    [[nodiscard]] VkImageMemoryBarrier2 barrier_to_present() const;
    [[nodiscard]] vk::resource::ImageViewRef view() const;
    [[nodiscard]] VkExtent2D extent() const;
private:
    vk::resource::ImageRef m_Image;
    vk::resource::ImageViewRef m_ImageView;
    swapchain_index_t m_Index;
    VkExtent2D m_Extent;
};
}    // namespace odin::graphics
