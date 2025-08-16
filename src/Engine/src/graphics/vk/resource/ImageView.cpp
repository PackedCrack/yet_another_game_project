#include "ImageView.hpp"

#include "../vulkan_defines.hpp"
//
//
namespace odin::graphics::vk::resource
{
ImageView::ImageView(const Device& device, VkImageView view)
    : m_View{ view }
    , m_Device{ device.handle() }
{}
ImageView::ImageView(const Device& device, const VkImageViewCreateInfo& info)
    : m_View{ VK_NULL_HANDLE }
    , m_Device{ device.handle() }
{
    ODIN_ASSERT(info.image != VK_NULL_HANDLE);
    VK_CHECK(vkCreateImageView(m_Device.handle, std::addressof(info), nullptr, &m_View), "Failed to create Vulkan ImageView");
}
ImageView::ImageView(DeviceRef device, const VkImageViewCreateInfo& info)
    : m_View{ VK_NULL_HANDLE }
    , m_Device{ device }
{
    ODIN_ASSERT(info.image != VK_NULL_HANDLE);
    VK_CHECK(vkCreateImageView(m_Device.handle, std::addressof(info), nullptr, &m_View), "Failed to create Vulkan ImageView");
}
ImageView::~ImageView()
{
    if (m_View != VK_NULL_HANDLE)
    {
        vkDestroyImageView(m_Device.handle, m_View, nullptr);
    }
}
ImageView::ImageView(ImageView&& other) noexcept
    : m_Device{ other.m_Device }
    , m_View{ VK_NULL_HANDLE }
{
    std::swap(m_View, other.m_View);
}
ImageView& ImageView::operator=(ImageView&& other) noexcept
{
    if (this != std::addressof(other))
    {
        m_Device = std::exchange(other.m_Device, m_Device);
        m_View = std::exchange(other.m_View, m_View);
    }

    return *this;
}
ImageViewRef ImageView::handle() const
{
    ODIN_ASSERT(m_View != VK_NULL_HANDLE);
    return ImageViewRef{ .handle = m_View };
}
}    // namespace odin::graphics::vk::resource
