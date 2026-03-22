//
// Created by qwerty on 01/03/2026.
//
#pragma once

#include "ImageRegistry.hpp"
#include "../../../FrameHandler.hpp"
#include "../../../ColorAttachment.hpp"
#include "../../../DepthAttachment.hpp"
#include "../../../vk/Allocator.hpp"
#include "../../../vk/Device.hpp"
//
//
namespace odin::graphics::registry::resource::image
{
struct DepthImage
{
    static constexpr std::uint64_t INVALID_ID = std::numeric_limits<std::uint64_t>::max();
    std::uint64_t id = INVALID_ID;
    VkExtent2D extent;
    VkSampleCountFlagBits samples;
    VkFormat depthFormat;
};
class AttachmentRegistry
{
public:
    AttachmentRegistry(vk::DeviceRef device, ImageRegistry& imageRegistry);
public:
    [[nodiscard]] DepthAttachment depth_image(const FrameContext& frame, const ColorAttachment& color);
private:
    [[nodiscard]] DepthImage make_depth_image(VkExtent2D extent, VkFormat format, VkSampleCountFlagBits samples);
    void expand_depth_attachment_storage(const FrameContext& frame);
private:
    vk::DeviceRef m_Device;
    std::reference_wrapper<ImageRegistry> m_Images;
    std::vector<DepthImage> m_DepthAttachments;
};
}    // namespace odin::graphics::registry::resource::image
