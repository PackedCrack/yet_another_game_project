//
// Created by qwerty on 07/09/2025.
//
#pragma once

#include "BufferHandle.hpp"
#include "../../../FrameHandler.hpp"
#include "../../../vk/Allocator.hpp"
#include "../../../vk/Device.hpp"
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
    BufferRegistry(vk::DeviceRef device,
                   const std::shared_ptr<vk::Allocator>& pAllocator,
                   const FrameHandler& frameHandler,
                   std::int32_t maxDraws,
                   std::int32_t maxInstances);
public:
    [[nodiscard]] std::reference_wrapper<const vk::resource::IndexBuffer> index_buffer() const;
    [[nodiscard]] std::reference_wrapper<const vk::resource::VertexBuffer> vertex_buffer() const;

    [[nodiscard]] BufferHandle<vk::resource::StorageBuffer> mesh_table() const;
    [[nodiscard]] BufferHandle<vk::resource::StorageBuffer> material_table() const;
    [[nodiscard]] BufferHandle<vk::resource::DynamicStorageBuffer> draw_variables() const;
    [[nodiscard]] BufferHandle<vk::resource::DynamicStorageBuffer> draw_commands() const;
    [[nodiscard]] BufferHandle<vk::resource::DynamicStorageBuffer> instance_base() const;
    [[nodiscard]] BufferHandle<vk::resource::DynamicStorageBuffer> instance_counter() const;
    [[nodiscard]] BufferHandle<vk::resource::DynamicStorageBuffer> instance_index() const;
    [[nodiscard]] BufferHandle<vk::resource::DynamicStorageBuffer> instance_info() const;
    [[nodiscard]] BufferHandle<vk::resource::DynamicUniformBuffer> camera_data() const;
private:
    std::unique_ptr<vk::resource::IndexBuffer> m_pIndexBuffer;
    std::unique_ptr<vk::resource::VertexBuffer> m_pVertexBuffer;

    std::shared_ptr<vk::resource::StorageBuffer> m_pMeshTable;
    std::shared_ptr<vk::resource::StorageBuffer> m_pMaterialTable;
    std::shared_ptr<vk::resource::DynamicStorageBuffer> m_pDrawVariables;
    std::shared_ptr<vk::resource::DynamicStorageBuffer> m_pDrawCommands;
    std::shared_ptr<vk::resource::DynamicStorageBuffer> m_pInstanceBase;
    std::shared_ptr<vk::resource::DynamicStorageBuffer> m_pInstanceCounter;
    std::shared_ptr<vk::resource::DynamicStorageBuffer> m_pInstanceIndex;
    std::shared_ptr<vk::resource::DynamicStorageBuffer> m_pInstanceInfo;
    std::shared_ptr<vk::resource::DynamicUniformBuffer> m_pCameraData;

    std::unordered_map<std::string, std::shared_ptr<vk::resource::DynamicStorageBuffer>> m_DynSSBO;
    std::unordered_map<std::string, std::shared_ptr<vk::resource::DynamicUniformBuffer>> m_DynUBO;
    std::unordered_map<std::string, std::shared_ptr<vk::resource::StorageBuffer>> m_SSBO;
};
}    // namespace odin::graphics::registry::resource::buffer
