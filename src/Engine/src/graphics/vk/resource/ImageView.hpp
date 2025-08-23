//
// Created by qwerty on 13/08/2025.
//
#pragma once

#include "../Device.hpp"
//
//
namespace odin::graphics::vk::resource
{
struct ImageViewRef
{
    VkImageView handle;
};
class ImageView
{
public:
    ImageView(const Device& device, VkImageView view);
    ImageView(const Device& device, const VkImageViewCreateInfo& info);
    ImageView(DeviceRef device, const VkImageViewCreateInfo& info);
    ~ImageView();
    ImageView(const ImageView& other) = delete;
    ImageView(ImageView&& other) noexcept;
    ImageView& operator=(const ImageView& other) = delete;
    ImageView& operator=(ImageView&& other) noexcept;
public:
    [[nodiscard]] ImageViewRef handle() const;
private:
    VkImageView m_View = VK_NULL_HANDLE;
    DeviceRef m_Device;
};
}    // namespace odin::graphics::vk::resource
