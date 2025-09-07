//
// Created by qwerty on 31/08/2025.
//
#include "descriptor_presets.hpp"

#include "vk/pipeline/DescriptorWriter.hpp"
#include "registry/pipeline/PipelineRegistry.hpp"
#include "registry/pipeline/RequestBuilder.hpp"
//
//
namespace odin::graphics
{
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
//
//
GlobalDescriptors::GlobalDescriptors(vk::DeviceRef device,
                                     registry::resource::ResourceRegistry& resources,
                                     registry::pipeline::PipelineRegistry& pipelines)
    : m_Set{ VK_NULL_HANDLE }
    , m_MeshTable{ resources.storage_buffer(registry::resource::ResourceRegistry::SSBO_MESH_TABLE) }
    , m_MaterialTable{ resources.storage_buffer(registry::resource::ResourceRegistry::SSBO_MATERIAL_TABLE) }
{
    registry::pipeline::RequestBuilder builder{};
    builder = global_preset(builder);
    registry::pipeline::Request request = builder.build();

    std::vector<registry::pipeline::descriptors::DescriptorSetLayoutKey> layouts = pipelines.make_descriptor_layouts(request);
    ODIN_ASSERT(layouts.size() == 1);

    m_Set = pipelines.allocate_descriptor_set(layouts.front());

    vk::pipeline::DescriptorWriter writer{ device, m_Set };
    writer.add_buffer(GLOBAL_SET_BIND_ID_MESH_INFO, m_MeshTable.to_view());
    writer.add_buffer(GLOBAL_SET_BIND_ID_MATERIAL_TABLE, m_MaterialTable.to_view());
    writer.write_descriptor_set();
}
//
//
IndirectDescriptors::IndirectDescriptors(vk::DeviceRef device,
                                         registry::resource::ResourceRegistry& resources,
                                         registry::pipeline::PipelineRegistry& pipelines)
    : m_Set{ VK_NULL_HANDLE }
    , m_DrawCount{ resources.dynamic_storage_buffer(registry::resource::ResourceRegistry::DYN_SSBO_DRAW_COUNT) }
    , m_DrawArgs{ resources.dynamic_storage_buffer(registry::resource::ResourceRegistry::DYN_SSBO_DRAW_ARGS) }
    , m_InstanceBase{ resources.dynamic_storage_buffer(registry::resource::ResourceRegistry::DYN_SSBO_INSTANCE_BASE) }
    , m_InstanceCounter{ resources.dynamic_storage_buffer(registry::resource::ResourceRegistry::DYN_SSBO_INSTANCE_COUNTER) }
    , m_InstanceIndex{ resources.dynamic_storage_buffer(registry::resource::ResourceRegistry::DYN_SSBO_INSTANCE_INDEX) }
    , m_InstanceInfo{ resources.dynamic_storage_buffer(registry::resource::ResourceRegistry::DYN_SSBO_INSTANCE_INFO) }
    , m_CameraData{ resources.dynamic_uniform_buffer(registry::resource::ResourceRegistry::DYN_UBO_CAMERA_DATA) }
{
    registry::pipeline::RequestBuilder builder{};
    builder = indirect_preset(builder);
    registry::pipeline::Request request = builder.build();

    std::vector<registry::pipeline::descriptors::DescriptorSetLayoutKey> layouts = pipelines.make_descriptor_layouts(request);
    ODIN_ASSERT(layouts.size() == 1);

    m_Set = pipelines.allocate_descriptor_set(layouts.front());
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
}    // namespace odin::graphics
