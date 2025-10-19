//
// Created by qwerty on 07/09/2025.
//
#pragma once

#include "DescriptorSet.hpp"
#include "../vk/Device.hpp"
#include "../registry/pipeline/PipelineRegistry.hpp"
#include "../registry/pipeline/RequestBuilder.hpp"
#include "../registry/resource/buffer/DynamicBufferHandle.hpp"
#include "../registry/resource/ResourceRegistry.hpp"
//
//
namespace odin::graphics::descriptors
{
class Indirect : public DescriptorSet<Indirect>
{
    using DynamicStorageBuffer = vk::resource::DynamicStorageBuffer;
    using DynamicUniformBuffer = vk::resource::DynamicUniformBuffer;
    using DynamicSSBOHandle = registry::resource::buffer::DynamicBufferHandle<DynamicStorageBuffer>;
    using DynamicUBOHandle = registry::resource::buffer::DynamicBufferHandle<DynamicUniformBuffer>;
public:
    Indirect(vk::DeviceRef device, registry::resource::ResourceRegistry& resources, registry::pipeline::PipelineRegistry& pipelines);
public:
    void bind(vk::CommandBufferRef cmdBuffer,
              const vk::pipeline::PipelineLayoutRef layout,
              VkShaderStageFlags stages,
              std::uint64_t frameID) const;
    [[nodiscard]] registry::resource::buffer::BindView view_draw_count(std::uint64_t frameID) const;
    [[nodiscard]] registry::resource::buffer::BindView view_draw_args(std::uint64_t frameID) const;
    [[nodiscard]] registry::resource::buffer::BindView view_instance_base(std::uint64_t frameID) const;
    [[nodiscard]] registry::resource::buffer::BindView view_instance_counter(std::uint64_t frameID) const;
    [[nodiscard]] registry::resource::buffer::BindView view_instance_index(std::uint64_t frameID) const;
    [[nodiscard]] registry::resource::buffer::BindView view_instance_info(std::uint64_t frameID) const;
    [[nodiscard]] registry::resource::buffer::BindView view_camera_data(std::uint64_t frameID) const;
private:
    std::array<std::uint32_t, 7> dynamic_offsets(std::uint64_t frameID) const;
private:
    DynamicSSBOHandle m_DrawCount;
    DynamicSSBOHandle m_DrawArgs;
    DynamicSSBOHandle m_InstanceBase;
    DynamicSSBOHandle m_InstanceCounter;
    DynamicSSBOHandle m_InstanceIndex;
    DynamicSSBOHandle m_InstanceInfo;
    DynamicUBOHandle m_CameraData;
};
[[nodiscard]] registry::pipeline::RequestBuilder& indirect_preset(registry::pipeline::RequestBuilder& builder);
}    // namespace odin::graphics::descriptors
