//
// Created by qwerty on 01/03/2026.
//
#pragma once

#include "../../../vk/Allocator.hpp"
#include "../../../vk/Device.hpp"
#include "../../../vk/resource/Image.hpp"
#include "../../../vk/resource/ImageView.hpp"

#include <unordered_map>
#include <cstdint>
//
//
namespace odin::graphics::registry::resource::image
{
struct ViewDescription
{
    VkImageViewType type;
    VkFormat format;
    VkImageSubresourceRange range;
    bool operator==(const ViewDescription& other) const noexcept
    {
        return type == other.type && format == other.format && range.aspectMask == other.range.aspectMask &&
               range.baseMipLevel == other.range.baseMipLevel && range.levelCount == other.range.levelCount &&
               range.baseArrayLayer == other.range.baseArrayLayer && range.layerCount == other.range.layerCount;
    }
    bool operator!=(const ViewDescription& other) const noexcept
    {
        return type != other.type || format != other.format || range.aspectMask != other.range.aspectMask ||
               range.baseMipLevel != other.range.baseMipLevel || range.levelCount != other.range.levelCount ||
               range.baseArrayLayer != other.range.baseArrayLayer || range.layerCount != other.range.layerCount;
    }
};
struct ViewDescriptionHasher
{
    size_t operator()(const ViewDescription& desc) const noexcept
    {
        using Hasher = std::hash<std::uint64_t>;

        std::uint64_t value = 0;
        value ^= Hasher{}(static_cast<std::uint64_t>(desc.type)) + 0x9e'37'79'b9'7f'4a'7c'15ull + (value << 6) + (value >> 2);
        value ^= Hasher{}(static_cast<std::uint64_t>(desc.format)) + 0x9e'37'79'b9'7f'4a'7c'15ull + (value << 6) + (value >> 2);
        value ^= Hasher{}(static_cast<std::uint64_t>(desc.range.aspectMask)) + 0x9e'37'79'b9'7f'4a'7c'15ull + (value << 6) + (value >> 2);
        value ^= Hasher{}(static_cast<std::uint64_t>(desc.range.baseMipLevel)) + 0x9e'37'79'b9'7f'4a'7c'15ull + (value << 6) + (value >> 2);
        value ^= Hasher{}(static_cast<std::uint64_t>(desc.range.levelCount)) + 0x9e'37'79'b9'7f'4a'7c'15ull + (value << 6) + (value >> 2);
        value ^=
            Hasher{}(static_cast<std::uint64_t>(desc.range.baseArrayLayer)) + 0x9e'37'79'b9'7f'4a'7c'15ull + (value << 6) + (value >> 2);
        value ^= Hasher{}(static_cast<std::uint64_t>(desc.range.layerCount)) + 0x9e'37'79'b9'7f'4a'7c'15ull + (value << 6) + (value >> 2);

        return value;
    }
};
struct ImageEntry
{
    vk::resource::Image image;
    std::unordered_map<ViewDescription, vk::resource::ImageView, ViewDescriptionHasher> views;
};
class ImageRegistry
{
public:
    [[nodiscard]] static std::unique_ptr<ImageRegistry> make(vk::DeviceRef device, std::shared_ptr<vk::Allocator> pAllocator);
private:
    ImageRegistry(vk::DeviceRef device, std::shared_ptr<vk::Allocator> pAllocator);
public:
    [[nodiscard]] std::uint64_t add_attachment_image(const VkImageCreateInfo& info);
    [[nodiscard]] std::uint64_t add_texture_image(const VkImageCreateInfo& info);
    [[nodiscard]] vk::resource::ImageViewRef view(std::uint64_t id, const ViewDescription& desc);
    [[nodiscard]] vk::resource::ImageRef image(std::uint64_t id) const;
    [[nodiscard]] bool contains(std::uint64_t id) const;
    void remove_image(std::uint64_t id);
private:
    [[nodiscard]] std::uint64_t add_image(ImageEntry& entry);
    [[nodiscard]] vk::resource::ImageViewRef emplace_view(ImageEntry& entry, const ViewDescription& desc);
private:
    vk::DeviceRef m_Device;
    std::shared_ptr<vk::Allocator> m_pAllocator;
    std::uint64_t m_NextID;
    std::unordered_map<std::uint64_t, ImageEntry> m_Images;
};
}    // namespace odin::graphics::registry::resource::image
