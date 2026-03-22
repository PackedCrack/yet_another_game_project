//
// Created by qwerty on 01/03/2026.
//
#include "AttachmentRegistry.hpp"

// debug
#include <debug/debug_defines.hpp>
//
//
namespace
{
using namespace odin::graphics::registry::resource::image;
//
//
[[nodiscard]] constexpr VkImageCreateInfo depth_image_info(VkExtent2D extent, VkFormat format, VkSampleCountFlagBits samples)
{
    VkImageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.imageType = VK_IMAGE_TYPE_2D;
    info.format = format;
    info.extent = VkExtent3D{ extent.width, extent.height, 1 };
    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.samples = samples;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    return info;
}
[[nodiscard]] constexpr ViewDescription make_depth_view_description(VkFormat format)
{
    VkImageSubresourceRange range{};
    range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    range.baseMipLevel = 0;
    range.levelCount = 1;
    range.baseArrayLayer = 0;
    range.layerCount = 1;

    ViewDescription desc{};
    desc.type = VK_IMAGE_VIEW_TYPE_2D;
    desc.format = format;
    desc.range = range;

    return desc;
}
[[nodiscard]] bool extent_match(const VkExtent2D& lhs, const VkExtent2D& rhs)
{
    if (lhs.height != rhs.height || lhs.width != rhs.width)
    {
        return false;
    }

    return true;
}
[[nodiscard]] bool depth_format_match(const DepthImage& image)
{
    // TODO: Get from cvar
    VkFormat format = VK_FORMAT_D32_SFLOAT_S8_UINT;
    return image.depthFormat == format;
}
[[nodiscard]] bool depth_samples_match(const DepthImage& image)
{
    // TODO: Get from cvar
    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
    return image.samples == samples;
}
[[nodiscard]] bool depth_image_out_of_date(const odin::graphics::ColorAttachment& color, const DepthImage& image)
{
    if (!extent_match(color.extent(), image.extent))
    {
        return true;
    }

    if (!depth_format_match(image))
    {
        return true;
    }

    if (!depth_samples_match(image))
    {
        return true;
    }

    return false;
}
}    // namespace
namespace odin::graphics::registry::resource::image
{
AttachmentRegistry::AttachmentRegistry(vk::DeviceRef device, ImageRegistry& imageRegistry)
    : m_Device{ device }
    , m_Images{ imageRegistry }
{}
DepthAttachment AttachmentRegistry::depth_image(const FrameContext& frame, const ColorAttachment& color)
{
    expand_depth_attachment_storage(frame);
    ODIN_ASSERT(frame.frame < m_DepthAttachments.size());

    // TODO: Get format and sample count from CVar
    VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
    VkSampleCountFlagBits depthSamples = VK_SAMPLE_COUNT_1_BIT;

    image::ImageRegistry& images = m_Images.get();
    DepthImage& image = m_DepthAttachments[frame.frame];
    if (image.id == DepthImage::INVALID_ID)
    {
        image = make_depth_image(color.extent(), depthFormat, depthSamples);
    }
    else if (depth_image_out_of_date(color, image))
    {
        if (images.contains(image.id) && image.id != DepthImage::INVALID_ID)
        {
            images.remove_image(image.id);
        }

        image = make_depth_image(color.extent(), depthFormat, depthSamples);
    }

    vk::resource::ImageRef depthImage = images.image(image.id);
    ViewDescription desc = make_depth_view_description(depthFormat);
    vk::resource::ImageViewRef view = images.view(image.id, desc);

    return DepthAttachment{ depthImage, view, image.extent };
}
DepthImage AttachmentRegistry::make_depth_image(VkExtent2D extent, VkFormat format, VkSampleCountFlagBits samples)
{
    VkImageCreateInfo info = depth_image_info(extent, format, samples);
    image::ImageRegistry& images = m_Images.get();
    std::uint64_t id = images.add_attachment_image(info);

    DepthImage di{};
    di.depthFormat = format;
    di.extent = extent;
    di.id = id;
    di.samples = samples;

    return di;
}
void AttachmentRegistry::expand_depth_attachment_storage(const FrameContext& frame)
{
    while (frame.frame >= m_DepthAttachments.size())
    {
        m_DepthAttachments.emplace_back();
    }
}
}    // namespace odin::graphics::registry::resource::image
