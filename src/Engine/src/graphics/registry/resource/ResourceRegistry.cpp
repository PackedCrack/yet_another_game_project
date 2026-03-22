//
// Created by qwerty on 26/08/2025.
//
#include "ResourceRegistry.hpp"
//
//
namespace odin::graphics::registry::resource
{
ResourceRegistry::ResourceRegistry(vk::DeviceRef device,
                                   std::shared_ptr<vk::Allocator> pAllocator,
                                   const FrameHandler& frameHandler,
                                   std::int32_t maxDraws,
                                   std::int32_t maxInstances)
    : m_pShaders{ shader::ShaderRegistry::make(device) }
    , m_Buffers{ device, pAllocator, frameHandler, maxDraws, maxInstances }
    , m_pImages{ image::ImageRegistry::make(device, std::move(pAllocator)) }
    , m_Attachments{ device, *m_pImages }
    , m_pMutex{ std::make_unique<mutex_t>() }
{}
shader::ShaderHandle ResourceRegistry::shader(std::string_view filename)
{
    return m_pShaders->shader(filename);
}
DepthAttachment ResourceRegistry::depth_attachment(const FrameContext& frame, const ColorAttachment& color)
{
    return m_Attachments.depth_image(frame, color);
}
buffer::BufferRegistry& ResourceRegistry::buffer_registry()
{
    return m_Buffers;
}
const buffer::BufferRegistry& ResourceRegistry::buffer_registry() const
{
    return m_Buffers;
}
image::ImageRegistry& ResourceRegistry::image_registry()
{
    return *m_pImages;
}
const image::ImageRegistry& ResourceRegistry::image_registry() const
{
    return *m_pImages;
}
const RenderResources ResourceRegistry::render_resources() const
{
    return RenderResources{ .indexBuffer = m_Buffers.index_buffer(),
                            .vertexBuffer = m_Buffers.vertex_buffer(),
                            .meshTable = m_Buffers.mesh_table() };
}
}    // namespace odin::graphics::registry::resource
