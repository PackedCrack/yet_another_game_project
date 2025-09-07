//
// Created by qwerty on 31/08/2025.
//
#pragma once

#include "gpu_types.hpp"
#include "vk/Device.hpp"
#include "registry/resource/buffer/BufferHandle.hpp"
#include "registry/resource/buffer/DynamicBufferHandle.hpp"
#include "registry/pipeline/RequestBuilder.hpp"
#include "registry/pipeline/PipelineRegistry.hpp"
#include "registry/resource/ResourceRegistry.hpp"
//
//
namespace odin::graphics
{
[[nodiscard]] registry::pipeline::RequestBuilder& global_preset(registry::pipeline::RequestBuilder& builder);
[[nodiscard]] registry::pipeline::RequestBuilder& indirect_preset(registry::pipeline::RequestBuilder& builder);
//
//
class GlobalDescriptors
{
    using StorageBuffer = vk::resource::StorageBuffer;
public:
    GlobalDescriptors(vk::DeviceRef device,
                      registry::resource::ResourceRegistry& resources,
                      registry::pipeline::PipelineRegistry& pipelines);
private:
    VkDescriptorSet m_Set;
    registry::resource::buffer::BufferHandle<StorageBuffer> m_MeshTable;
    registry::resource::buffer::BufferHandle<StorageBuffer> m_MaterialTable;
};
class IndirectDescriptors
{
    using DynamicStorageBuffer = vk::resource::DynamicStorageBuffer;
    using DynamicUniformBuffer = vk::resource::DynamicUniformBuffer;
public:
    IndirectDescriptors(vk::DeviceRef device,
                        registry::resource::ResourceRegistry& resources,
                        registry::pipeline::PipelineRegistry& pipelines);
private:
    VkDescriptorSet m_Set;
    registry::resource::buffer::DynamicBufferHandle<DynamicStorageBuffer> m_DrawCount;
    registry::resource::buffer::DynamicBufferHandle<DynamicStorageBuffer> m_DrawArgs;
    registry::resource::buffer::DynamicBufferHandle<DynamicStorageBuffer> m_InstanceBase;
    registry::resource::buffer::DynamicBufferHandle<DynamicStorageBuffer> m_InstanceCounter;
    registry::resource::buffer::DynamicBufferHandle<DynamicStorageBuffer> m_InstanceIndex;
    registry::resource::buffer::DynamicBufferHandle<DynamicStorageBuffer> m_InstanceInfo;
    registry::resource::buffer::DynamicBufferHandle<DynamicUniformBuffer> m_CameraData;
};
}    // namespace odin::graphics
