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
    // This makes me want to kms but I dont have a better solution right now
    static constexpr std::string_view SSBO_MESH_TABLE = "meshtable";
    static constexpr std::string_view SSBO_MATERIAL_TABLE = "materialtable";
    static constexpr std::string_view DYN_SSBO_DRAW_VARIABLES = "drawvariables";
    static constexpr std::string_view DYN_SSBO_DRAW_COMMANDS = "drawcommands";
    static constexpr std::string_view DYN_SSBO_INSTANCE_BASE = "instancebase";
    static constexpr std::string_view DYN_SSBO_INSTANCE_COUNTER = "instancecounter";
    static constexpr std::string_view DYN_SSBO_INSTANCE_INDEX = "instanceindex";
    static constexpr std::string_view DYN_SSBO_INSTANCE_INFO = "instanceinfo";
    static constexpr std::string_view DYN_UBO_CAMERA_DATA = "cameradata";
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
    [[nodiscard]] buffer::BufferHandle<vk::resource::StorageBuffer> storage_buffer(std::string_view key) const;
    [[nodiscard]] buffer::DynamicBufferHandle<vk::resource::DynamicStorageBuffer> dynamic_storage_buffer(std::string_view key) const;
    [[nodiscard]] buffer::DynamicBufferHandle<vk::resource::DynamicUniformBuffer> dynamic_uniform_buffer(std::string_view key) const;
    [[nodiscard]] const RenderResources render_resources() const;
private:
    std::unique_ptr<shader::ShaderRegistry> m_pShaders;
    buffer::BufferRegistry m_Buffers;
    Mutex m_pMutex;
};
}    // namespace odin::graphics::registry::resource
