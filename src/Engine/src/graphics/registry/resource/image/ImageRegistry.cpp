//
// Created by qwerty on 01/03/2026.
//
#include "ImageRegistry.hpp"

// debug
#include <debug/debug_defines.hpp>
//
//
namespace odin::graphics::registry::resource::image
{
std::unique_ptr<ImageRegistry> ImageRegistry::make(vk::DeviceRef device, std::shared_ptr<vk::Allocator> pAllocator)
{
    return std::make_unique<ImageRegistry>(ImageRegistry{ device, std::move(pAllocator) });
}
ImageRegistry::ImageRegistry(vk::DeviceRef device, std::shared_ptr<vk::Allocator> pAllocator)
    : m_Device{ device }
    , m_pAllocator{ std::move(pAllocator) }
    , m_NextID{ 1 }
    , m_Images{}
{}
std::uint64_t ImageRegistry::add_attachment_image(const VkImageCreateInfo& info)
{
    ImageEntry entry{ .image = m_pAllocator->create_image_attachment(info), .views = {} };
    return add_image(entry);
}
std::uint64_t ImageRegistry::add_texture_image(const VkImageCreateInfo& info)
{
    ImageEntry entry{ .image = m_pAllocator->create_image_texture(info), .views = {} };
    return add_image(entry);
}
vk::resource::ImageViewRef ImageRegistry::view(std::uint64_t id, const ViewDescription& desc)
{
    auto it = m_Images.find(id);
    ODIN_ASSERT(it != m_Images.end());
    ImageEntry& entry = it->second;

    auto viewIter = entry.views.find(desc);
    if (viewIter != entry.views.end())
    {
        return viewIter->second.handle();
    }

    return emplace_view(entry, desc);
}
vk::resource::ImageRef ImageRegistry::image(std::uint64_t id) const
{
    auto it = m_Images.find(id);
    ODIN_ASSERT(it != m_Images.end());
    const ImageEntry& entry = it->second;

    return entry.image.handle();
}
bool ImageRegistry::contains(std::uint64_t id) const
{
    return m_Images.contains(id);
}
void ImageRegistry::remove_image(std::uint64_t id)
{
    if (!m_Images.contains(id))
    {
        LOG_ERR("Tried to erase non existing image with ID 0x{:X}.", id);
        return;
    }

    decltype(m_Images)::size_type removed = m_Images.erase(id);
    if (removed < 1)
    {
        LOG_WARN("Failed to erase existing image with ID 0x{:X}.", id);
    }
}
std::uint64_t ImageRegistry::add_image(ImageEntry& entry)
{
    m_Images.emplace(m_NextID, std::move(entry));
    return m_NextID++;
}
vk::resource::ImageViewRef ImageRegistry::emplace_view(ImageEntry& entry, const ViewDescription& desc)
{
    VkImageViewCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = VK_NO_FLAGS;
    info.image = entry.image.handle().handle;
    info.viewType = desc.type;
    info.format = desc.format;
    info.subresourceRange = desc.range;

    vk::resource::ImageView v{ m_Device, info };
    auto [it, emplaced] = entry.views.emplace(desc, std::move(v));

    return it->second.handle();
}
}    // namespace odin::graphics::registry::resource::image
