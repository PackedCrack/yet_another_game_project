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
    VkImage image = VK_NULL_HANDLE;
    void* pAllocation = nullptr;    // This is pointer to VmaAllocation
};
struct ImageRef
{
    VkImage handle;
};
class Image
{
public:
    Image(std::shared_ptr<Allocator> pAllocator, AllocatedImage image, VkFormat format);
    ~Image();
    Image(const Image& other) = delete;
    Image(Image&& other) noexcept;
    Image& operator=(const Image& other) = delete;
    Image& operator=(Image&& other) noexcept;
private:
    VkImage m_Image = VK_NULL_HANDLE;
    void* m_pAllocation = nullptr;
    std::shared_ptr<Allocator> m_pAllocator = nullptr;
    VkFormat m_Format;
};
}    // namespace odin::graphics::vk::resource
