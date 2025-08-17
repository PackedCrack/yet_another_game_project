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
    ColorAttachment(vk::Swapchain::AcquiredImage acquiredImage);
public:
    [[nodiscard]] VkImageMemoryBarrier2 barrier_to_render() const;
    [[nodiscard]] VkImageMemoryBarrier2 barrier_to_present() const;
    [[nodiscard]] vk::resource::ImageViewRef view() const;
private:
    vk::resource::ImageRef m_Image;
    vk::resource::ImageViewRef m_ImageView;
    swapchain_index_t m_Index;
};
}    // namespace odin::graphics
