//
// Created by qwerty on 12/08/2025.
//
#pragma once


// vulkan
#include <vulkan/vulkan.h>
//
//
namespace odin::graphics::vk
{
class Allocator;
}    // namespace odin::graphics::vk
namespace odin::graphics::vk::resource
{
struct AllocatedImage
{
    VkImage handle = VK_NULL_HANDLE;
    void* pAllocation = nullptr;    // This is pointer to VmaAllocation
};
struct ImageRef
{
    VkImage handle;
};
class Image
{
public:
    Image(AllocatedImage image, std::function<void(AllocatedImage)> deleter, VkFormat format);
    ~Image();
    Image(const Image& other) = delete;
    Image(Image&& other) noexcept;
    Image& operator=(const Image& other) = delete;
    Image& operator=(Image&& other) noexcept;
public:
    [[nodiscard]] VkFormat format() const;
    [[nodiscard]] ImageRef handle() const;
private:
    AllocatedImage m_Image;
    std::function<void(AllocatedImage)> m_Deleter;
    VkFormat m_Format;
};
}    // namespace odin::graphics::vk::resource
