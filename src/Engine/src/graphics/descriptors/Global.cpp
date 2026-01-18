//
// Created by qwerty on 07/09/2025.
//
#include "Global.hpp"

#include "../gpu_types.hpp"
#include "../vk/pipeline/DescriptorWriter.hpp"
//
//
namespace
{
using namespace odin::graphics;
using namespace odin::graphics::descriptors;
[[nodiscard]] registry::pipeline::Request make_request()
{
    registry::pipeline::RequestBuilder builder{};
    builder = global_preset(builder);
    return builder.build();
}
}    // namespace
namespace odin::graphics::descriptors
{
Global::Global(vk::DeviceRef device, registry::resource::ResourceRegistry& resources, registry::pipeline::PipelineRegistry& pipelines)
    : DescriptorSet<Global>{ make_request(), pipelines }
    , m_MeshTable{ resources.storage_buffer(registry::resource::ResourceRegistry::SSBO_MESH_TABLE) }
    , m_MaterialTable{ resources.storage_buffer(registry::resource::ResourceRegistry::SSBO_MATERIAL_TABLE) }
{
    vk::pipeline::DescriptorWriter writer{ device, m_Set };
    writer.add_buffer(GLOBAL_SET_BIND_ID_MESH_INFO, m_MeshTable.to_view());
    writer.add_buffer(GLOBAL_SET_BIND_ID_MATERIAL_TABLE, m_MaterialTable.to_view());
    writer.write_descriptor_set();
}
void Global::bind(vk::CommandBufferRef cmdBuffer, const vk::pipeline::PipelineLayoutRef layout, VkShaderStageFlags stages) const
{
    // This is 100% a retarded driver bug..
    std::uint32_t offset{};
    DescriptorSet<Global>::bind(cmdBuffer, layout, stages, GLOBAL_SET_ID, std::addressof(offset), 0);
}
registry::resource::buffer::BindView Global::view_mesh_table() const
{
    return m_MeshTable.to_view();
}
registry::resource::buffer::BindView Global::view_material_table() const
{
    return m_MaterialTable.to_view();
}
//
//
registry::pipeline::RequestBuilder& global_preset(registry::pipeline::RequestBuilder& builder)
{
    using Descriptor = registry::pipeline::DescriptorType;
    using Stage = registry::pipeline::ShaderStage;

    // clang-format off
    builder.add_descriptor_layout(GLOBAL_SET_ID,
                                  GLOBAL_SET_BIND_ID_MATERIAL_TABLE,
                                  Descriptor::storageBuffer,
                                  true,
                                  Stage::compute,
                                  Stage::vertex,
                                  Stage::fragment);

    builder.add_descriptor_layout(GLOBAL_SET_ID,
                                  GLOBAL_SET_BIND_ID_MESH_INFO,
                                  Descriptor::storageBuffer,
                                  Stage::compute,
                                  Stage::vertex,
                                  Stage::fragment);
    // clang-format on
    return builder;
}
}    // namespace odin::graphics::descriptors
