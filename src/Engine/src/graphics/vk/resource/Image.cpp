//
// Created by qwerty on 12/08/2025.
//
#include "Image.hpp"

#include "../Allocator.hpp"
#include "../vulkan_defines.hpp"
//
//
namespace odin::graphics::vk::resource
{
Image::Image(AllocatedImage image, std::function<void(AllocatedImage)> deleter, VkFormat format)
    : m_Image{ image }
    , m_Deleter{ std::move(deleter) }
    , m_Format{ format }
{}
Image::~Image()
{
    if (m_Image.handle != VK_NULL_HANDLE)
    {
        ODIN_ASSERT(m_Image.pAllocation != nullptr);
        m_Deleter(m_Image);
    }
}
Image::Image(Image&& other) noexcept
    : m_Image{ VK_NULL_HANDLE }
    , m_Deleter{ nullptr }
    , m_Format{ other.m_Format }
{
    std::swap(m_Image, other.m_Image);
    std::swap(m_Deleter, other.m_Deleter);
}
Image& Image::operator=(Image&& other) noexcept
{
    if (this != std::addressof(other))
    {
        m_Image = std::exchange(other.m_Image, m_Image);
        m_Deleter = std::exchange(other.m_Deleter, std::move(m_Deleter));
        m_Format = std::exchange(other.m_Format, m_Format);
    }

    return *this;
}
}    // namespace odin::graphics::vk::resource
