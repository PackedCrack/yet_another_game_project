//
// Created by qwerty on 26/08/2025.
//
#pragma once

#include "../../FrameHandler.hpp"
#include "shader/ShaderRegistry.hpp"
#include "buffer/BufferRegistry.hpp"
#include "../../vk/Device.hpp"
//
//
namespace odin::graphics::registry::resource
{
struct RenderResources
{
    std::reference_wrapper<const vk::resource::IndexBuffer> indexBuffer;
    std::reference_wrapper<const vk::resource::VertexBuffer> vertexBuffer;
    buffer::BufferHandle<vk::resource::StorageBuffer> meshTable;
};
class ResourceRegistry
{
    using mutex_t = std::mutex;
    using Mutex = std::unique_ptr<mutex_t>;
public:
    ResourceRegistry(vk::DeviceRef device,
                     std::shared_ptr<vk::Allocator> pAllocator,
                     const FrameHandler& frameHandler,
                     std::int32_t maxDraws,
                     std::int32_t maxInstances);
public:
    [[nodiscard]] shader::ShaderHandle shader(std::string_view filename);
    [[nodiscard]] std::reference_wrapper<const vk::resource::IndexBuffer> index_buffer() const;
    [[nodiscard]] std::reference_wrapper<const vk::resource::VertexBuffer> vertex_buffer() const;
    [[nodiscard]] const buffer::BufferRegistry& buffer_registry() const;
    [[nodiscard]] const RenderResources render_resources() const;
private:
    std::unique_ptr<shader::ShaderRegistry> m_pShaders;
    buffer::BufferRegistry m_Buffers;
    Mutex m_pMutex;
};
}    // namespace odin::graphics::registry::resource
