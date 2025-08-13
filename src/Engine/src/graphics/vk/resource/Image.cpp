#include "Image.hpp"

#include "../Allocator.hpp"
#include "../vulkan_defines.hpp"
//
//
namespace odin::graphics::vk::resource
{
Image::Image(std::shared_ptr<Allocator> pAllocator, AllocatedImage image, VkFormat format)
    : m_Image{ image.image }
    , m_pAllocation{ image.pAllocation }
    , m_pAllocator{ std::move(pAllocator) }
    , m_Format{ format }
{}
Image::~Image()
{
    if (m_Image != VK_NULL_HANDLE)
    {
        ODIN_ASSERT(m_pAllocation != nullptr);
        m_pAllocator->destroy_image(m_Image, m_pAllocation);
    }
}
Image::Image(Image&& other) noexcept
    : m_Image{ std::exchange(other.m_Image, m_Image) }
    , m_pAllocation{ std::exchange(other.m_pAllocation, m_pAllocation) }
    , m_pAllocator{ std::exchange(other.m_pAllocator, m_pAllocator) }
    , m_Format{ std::exchange(other.m_Format, m_Format) }
{}
Image& Image::operator=(Image&& other) noexcept
{
    if (this != std::addressof(other))
    {
        m_Image = std::exchange(other.m_Image, m_Image);
        m_pAllocation = std::exchange(other.m_pAllocation, m_pAllocation);
        m_pAllocator = std::exchange(other.m_pAllocator, m_pAllocator);
        m_Format = std::exchange(other.m_Format, m_Format);
    }

    return *this;
}
}    // namespace odin::graphics::vk::resource
