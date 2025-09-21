//
// Created by qwerty on 07/09/2025.
//
#include "Indirect.hpp"


#include "../gpu_types.hpp"
#include "../vk/pipeline/DescriptorWriter.hpp"
//
//

//
//
namespace
{
using namespace odin::graphics;
using namespace odin::graphics::descriptors;
//
//
[[nodiscard]] registry::pipeline::Request make_request()
{
    registry::pipeline::RequestBuilder builder{};
    builder = indirect_preset(builder);
    return builder.build();
}
}    // namespace
namespace odin::graphics::descriptors
{
Indirect::Indirect(vk::DeviceRef device, registry::resource::ResourceRegistry& resources, registry::pipeline::PipelineRegistry& pipelines)
    : DescriptorSet<Indirect>{ make_request(), pipelines }
    , m_DrawCount{ resources.dynamic_storage_buffer(registry::resource::ResourceRegistry::DYN_SSBO_DRAW_COUNT) }
    , m_DrawArgs{ resources.dynamic_storage_buffer(registry::resource::ResourceRegistry::DYN_SSBO_DRAW_ARGS) }
    , m_InstanceBase{ resources.dynamic_storage_buffer(registry::resource::ResourceRegistry::DYN_SSBO_INSTANCE_BASE) }
    , m_InstanceCounter{ resources.dynamic_storage_buffer(registry::resource::ResourceRegistry::DYN_SSBO_INSTANCE_COUNTER) }
    , m_InstanceIndex{ resources.dynamic_storage_buffer(registry::resource::ResourceRegistry::DYN_SSBO_INSTANCE_INDEX) }
    , m_InstanceInfo{ resources.dynamic_storage_buffer(registry::resource::ResourceRegistry::DYN_SSBO_INSTANCE_INFO) }
    , m_CameraData{ resources.dynamic_uniform_buffer(registry::resource::ResourceRegistry::DYN_UBO_CAMERA_DATA) }
{
    vk::pipeline::DescriptorWriter writer{ device, m_Set };

    std::uint64_t frameID = 0;
    writer.add_buffer(INDIRECT_SET_BIND_ID_CAMERA_DATA, m_CameraData.to_view(frameID));
    writer.add_buffer(INDIRECT_SET_BIND_ID_DRAW_ARGS, m_DrawArgs.to_view(frameID));
    writer.add_buffer(INDIRECT_SET_BIND_ID_DRAW_COUNT, m_DrawCount.to_view(frameID));
    writer.add_buffer(INDIRECT_SET_BIND_ID_DRAW_INSTANCE_BASE, m_InstanceBase.to_view(frameID));
    writer.add_buffer(INDIRECT_SET_BIND_ID_DRAW_INSTANCE_COUNTER, m_InstanceCounter.to_view(frameID));
    writer.add_buffer(INDIRECT_SET_BIND_ID_DRAW_INSTANCE_INDEX, m_InstanceIndex.to_view(frameID));
    writer.add_buffer(INDIRECT_SET_BIND_ID_INSTANCE_INFO, m_InstanceInfo.to_view(frameID));

    writer.write_descriptor_set();
}
void Indirect::bind(vk::CommandBufferRef cmdBuffer,
                    const vk::pipeline::PipelineLayoutRef layout,
                    VkShaderStageFlags stages,
                    std::uint64_t frameID) const
{
    std::array<std::uint32_t, 7> offsets = dynamic_offsets(frameID);
    DescriptorSet<Indirect>::bind(cmdBuffer, layout, stages, INDIRECT_SET_ID, offsets.data(), static_cast<std::uint32_t>(offsets.size()));
}
std::array<std::uint32_t, 7> Indirect::dynamic_offsets(std::uint64_t frameID) const
{
    return { static_cast<std::uint32_t>(m_DrawCount->offset(frameID)),     static_cast<std::uint32_t>(m_DrawArgs->offset(frameID)),
             static_cast<std::uint32_t>(m_InstanceBase->offset(frameID)),  static_cast<std::uint32_t>(m_InstanceCounter->offset(frameID)),
             static_cast<std::uint32_t>(m_InstanceIndex->offset(frameID)), static_cast<std::uint32_t>(m_InstanceInfo->offset(frameID)),
             static_cast<std::uint32_t>(m_CameraData->offset(frameID)) };
}
//
//
registry::pipeline::RequestBuilder& indirect_preset(registry::pipeline::RequestBuilder& builder)
{
    using Descriptor = registry::pipeline::DescriptorType;
    using Stage = registry::pipeline::ShaderStage;

    // clang-format off
    builder.add_descriptor_layout(INDIRECT_SET_ID,
                                  INDIRECT_SET_BIND_ID_DRAW_COUNT,
                                  Descriptor::dynamicStorageBuffer,
                                  Stage::compute);

    builder.add_descriptor_layout(INDIRECT_SET_ID,
                                  INDIRECT_SET_BIND_ID_DRAW_ARGS,
                                  Descriptor::dynamicStorageBuffer,
                                  Stage::compute);

    builder.add_descriptor_layout(INDIRECT_SET_ID,
                                  INDIRECT_SET_BIND_ID_DRAW_INSTANCE_BASE,
                                  Descriptor::dynamicStorageBuffer,
                                  Stage::compute);

    builder.add_descriptor_layout(INDIRECT_SET_ID,
                                  INDIRECT_SET_BIND_ID_DRAW_INSTANCE_COUNTER,
                                  Descriptor::dynamicStorageBuffer,
                                  Stage::compute);

    builder.add_descriptor_layout(INDIRECT_SET_ID,
                                  INDIRECT_SET_BIND_ID_DRAW_INSTANCE_INDEX,
                                  Descriptor::dynamicStorageBuffer,
                                  Stage::compute,
                                  Stage::vertex,
                                  Stage::fragment);

    builder.add_descriptor_layout(INDIRECT_SET_ID,
                                  INDIRECT_SET_BIND_ID_INSTANCE_INFO,
                                  Descriptor::dynamicStorageBuffer,
                                  Stage::compute,
                                  Stage::vertex,
                                  Stage::fragment);

    builder.add_descriptor_layout(INDIRECT_SET_ID,
                                  INDIRECT_SET_BIND_ID_CAMERA_DATA,
                                  Descriptor::dynamicUniformBuffer,
                                  Stage::compute,
                                  Stage::vertex,
                                  Stage::fragment);
    // clang-format on
    return builder;
}
}    // namespace odin::graphics::descriptors
