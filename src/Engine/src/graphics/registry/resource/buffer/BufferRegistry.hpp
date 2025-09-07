//
// Created by qwerty on 07/09/2025.
//
#pragma once

#include "BufferHandle.hpp"
#include "DynamicBufferHandle.hpp"
#include "../../../FrameHandler.hpp"
#include "../../../vk/Allocator.hpp"
#include "../../../vk/resource/DynamicStorageBuffer.hpp"
#include "../../../vk/resource/DynamicUniformBuffer.hpp"
#include "../../../vk/resource/IndexBuffer.hpp"
#include "../../../vk/resource/StorageBuffer.hpp"
#include "../../../vk/resource/VertexBuffer.hpp"
//
//
namespace odin::graphics::registry::resource::buffer
{
class BufferRegistry
{
public:
    BufferRegistry(const std::shared_ptr<vk::Allocator>& pAllocator,
                   const FrameHandler& frameHandler,
                   std::int32_t maxDraws,
                   std::int32_t maxInstances);
public:
    [[nodiscard]] std::reference_wrapper<const vk::resource::IndexBuffer> index_buffer() const;
    [[nodiscard]] std::reference_wrapper<const vk::resource::VertexBuffer> vertex_buffer() const;
    [[nodiscard]] BufferHandle<vk::resource::StorageBuffer> get_storage_buffer(std::string_view key) const;
    [[nodiscard]] DynamicBufferHandle<vk::resource::DynamicStorageBuffer> get_dynamic_storage_buffer(std::string_view key) const;
    [[nodiscard]] DynamicBufferHandle<vk::resource::DynamicUniformBuffer> get_dynamic_uniform_buffer(std::string_view key) const;
private:
    void make_buffers(const std::shared_ptr<vk::Allocator>& pAllocator,
                      std::int32_t numFramesInFlight,
                      std::int32_t maxDraws,
                      std::int32_t maxInstances);
private:
    std::unique_ptr<vk::resource::IndexBuffer> m_pIndexBuffer;
    std::unique_ptr<vk::resource::VertexBuffer> m_pVertexBuffer;
    std::unordered_map<std::string, std::shared_ptr<vk::resource::DynamicStorageBuffer>> m_DynSSBO;
    std::unordered_map<std::string, std::shared_ptr<vk::resource::DynamicUniformBuffer>> m_DynUBO;
    std::unordered_map<std::string, std::shared_ptr<vk::resource::StorageBuffer>> m_SSBO;
};
}    // namespace odin::graphics::registry::resource::buffer
