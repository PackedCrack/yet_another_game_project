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
    , m_DrawVariables{ resources.buffer_registry().draw_variables() }
    , m_DrawCommands{ resources.buffer_registry().draw_commands() }
    , m_InstanceBase{ resources.buffer_registry().instance_base() }
    , m_InstanceCounter{ resources.buffer_registry().instance_counter() }
    , m_InstanceIndex{ resources.buffer_registry().instance_index() }
    , m_InstanceInfo{ resources.buffer_registry().instance_info() }
    , m_CameraData{ resources.buffer_registry().camera_data() }
{
    vk::pipeline::DescriptorWriter writer{ device, m_Set };

    std::uint64_t frameID = 0;
    writer.add_buffer(INDIRECT_SET_BIND_ID_CAMERA_DATA, m_CameraData.to_view(frameID));
    writer.add_buffer(INDIRECT_SET_BIND_ID_DRAW_COMMANDS, m_DrawCommands.to_view(frameID));
    writer.add_buffer(INDIRECT_SET_BIND_ID_DRAW_VARIABLES, m_DrawVariables.to_view(frameID));
    writer.add_buffer(INDIRECT_SET_BIND_ID_INSTANCE_BASE, m_InstanceBase.to_view(frameID));
    writer.add_buffer(INDIRECT_SET_BIND_ID_INSTANCE_COUNTER, m_InstanceCounter.to_view(frameID));
    writer.add_buffer(INDIRECT_SET_BIND_ID_INSTANCE_INDEX, m_InstanceIndex.to_view(frameID));
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
registry::resource::buffer::BindView Indirect::view_draw_variables(std::uint64_t frameID) const
{
    return m_DrawVariables.to_view(frameID);
}
registry::resource::buffer::BindView Indirect::view_draw_commands(std::uint64_t frameID) const
{
    return m_DrawCommands.to_view(frameID);
}
registry::resource::buffer::BindView Indirect::view_instance_base(std::uint64_t frameID) const
{
    return m_InstanceBase.to_view(frameID);
}
registry::resource::buffer::BindView Indirect::view_instance_counter(std::uint64_t frameID) const
{
    return m_InstanceCounter.to_view(frameID);
}
registry::resource::buffer::BindView Indirect::view_instance_index(std::uint64_t frameID) const
{
    return m_InstanceIndex.to_view(frameID);
}
registry::resource::buffer::BindView Indirect::view_instance_info(std::uint64_t frameID) const
{
    return m_InstanceInfo.to_view(frameID);
}
registry::resource::buffer::BindView Indirect::view_camera_data(std::uint64_t frameID) const
{
    return m_CameraData.to_view(frameID);
}
std::array<std::uint32_t, 7> Indirect::dynamic_offsets(std::uint64_t frameID) const
{
    return { static_cast<std::uint32_t>(m_DrawVariables->offset(frameID)), static_cast<std::uint32_t>(m_DrawCommands->offset(frameID)),
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
                                  INDIRECT_SET_BIND_ID_DRAW_VARIABLES,
                                  Descriptor::dynamicStorageBuffer,
                                  Stage::compute);

    builder.add_descriptor_layout(INDIRECT_SET_ID,
                                  INDIRECT_SET_BIND_ID_DRAW_COMMANDS,
                                  Descriptor::dynamicStorageBuffer,
                                  Stage::compute);

    builder.add_descriptor_layout(INDIRECT_SET_ID,
                                  INDIRECT_SET_BIND_ID_INSTANCE_BASE,
                                  Descriptor::dynamicStorageBuffer,
                                  Stage::compute);

    builder.add_descriptor_layout(INDIRECT_SET_ID,
                                  INDIRECT_SET_BIND_ID_INSTANCE_COUNTER,
                                  Descriptor::dynamicStorageBuffer,
                                  Stage::compute);

    builder.add_descriptor_layout(INDIRECT_SET_ID,
                                  INDIRECT_SET_BIND_ID_INSTANCE_INDEX,
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
