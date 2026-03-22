#include "DepthAttachment.hpp"
//
//
namespace odin::graphics
{
vk::resource::ImageRef DepthAttachment::image() const
{
    return m_Image;
}
vk::resource::ImageViewRef DepthAttachment::view() const
{
    return m_View;
}
std::uint32_t DepthAttachment::width() const
{
    return m_Extent.width;
}
std::uint32_t DepthAttachment::height() const
{
    return m_Extent.height;
}
}    // namespace odin::graphics
