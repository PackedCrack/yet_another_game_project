#pragma once

#include "../Allocator.hpp"
// vulkan
#include <vulkan/vulkan.h>
//
//
namespace odin::graphics::vk
{
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
}    // namespace odin::graphics::vk
