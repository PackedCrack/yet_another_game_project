//
// Created by qwerty on 07/09/2025.
//
#pragma once


#include "DescriptorSet.hpp"
#include "../vk/Device.hpp"
#include "../registry/pipeline/PipelineRegistry.hpp"
#include "../registry/pipeline/RequestBuilder.hpp"
#include "../registry/resource/buffer/BufferHandle.hpp"
#include "../registry/resource/ResourceRegistry.hpp"
//
//
namespace odin::graphics::descriptors
{
class Global : public DescriptorSet<Global>
{
    using StorageBuffer = vk::resource::StorageBuffer;
    using SSBOHandle = registry::resource::buffer::BufferHandle<StorageBuffer>;
public:
    Global(vk::DeviceRef device, registry::resource::ResourceRegistry& resources, registry::pipeline::PipelineRegistry& pipelines);
public:
    void bind(vk::CommandBufferRef cmdBuffer, const vk::pipeline::PipelineLayoutRef layout, VkShaderStageFlags stages) const;
    [[nodiscard]] registry::resource::buffer::BindView view_mesh_table() const;
    [[nodiscard]] registry::resource::buffer::BindView view_material_table() const;
private:
    SSBOHandle m_MeshTable;
    SSBOHandle m_MaterialTable;
};
[[nodiscard]] registry::pipeline::RequestBuilder& global_preset(registry::pipeline::RequestBuilder& builder);
}    // namespace odin::graphics::descriptors
