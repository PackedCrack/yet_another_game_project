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
    , m_Buffers{ std::move(pAllocator), frameHandler, maxDraws, maxInstances }
    , m_pMutex{ std::make_unique<mutex_t>() }
{}
shader::ShaderHandle ResourceRegistry::shader(std::string_view filename)
{
    return m_pShaders->shader(filename);
}
std::reference_wrapper<const vk::resource::IndexBuffer> ResourceRegistry::index_buffer() const
{
    return m_Buffers.index_buffer();
}
std::reference_wrapper<const vk::resource::VertexBuffer> ResourceRegistry::vertex_buffer() const
{
    return m_Buffers.vertex_buffer();
}
buffer::BufferHandle<vk::resource::StorageBuffer> ResourceRegistry::storage_buffer(std::string_view key) const
{
    return m_Buffers.get_storage_buffer(key);
}
buffer::DynamicBufferHandle<vk::resource::DynamicStorageBuffer> ResourceRegistry::dynamic_storage_buffer(std::string_view key) const
{
    return m_Buffers.get_dynamic_storage_buffer(key);
}
buffer::DynamicBufferHandle<vk::resource::DynamicUniformBuffer> ResourceRegistry::dynamic_uniform_buffer(std::string_view key) const
{
    return m_Buffers.get_dynamic_uniform_buffer(key);
}
}    // namespace odin::graphics::registry::resource
