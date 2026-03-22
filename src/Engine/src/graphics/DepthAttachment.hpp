//
// Created by qwerty on 22/03/2026.
//
#pragma once


#include "vk/resource/Image.hpp"
#include "vk/resource/ImageView.hpp"
namespace odin::graphics
{
class DepthAttachment
{
public:
    DepthAttachment(vk::resource::ImageRef depth, vk::resource::ImageViewRef view, VkExtent2D extent)
        : m_Image{ depth }
        , m_View{ view }
        , m_Extent{ extent }
    {}
    [[nodiscard]] vk::resource::ImageRef image() const;
    [[nodiscard]] vk::resource::ImageViewRef view() const;
    [[nodiscard]] std::uint32_t width() const;
    [[nodiscard]] std::uint32_t height() const;
public:
    vk::resource::ImageRef m_Image;
    vk::resource::ImageViewRef m_View;
    VkExtent2D m_Extent;
};
}    // namespace odin::graphics
