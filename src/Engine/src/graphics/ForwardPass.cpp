//
// Created by qwerty on 26/07/2025.
//
#include "ForwardPass.hpp"

#include "gpu_types.hpp"
#include "registry/pipeline/Request.hpp"
#include "registry/pipeline/RequestBuilder.hpp"
#include "registry/resource/ShaderHandle.hpp"
//
//
namespace
{
using namespace odin::graphics;
using ShaderStages = registry::pipeline::ShaderStages;
using ShaderHandle = registry::resource::ShaderHandle;
//
//
[[nodiscard]] ShaderStages make_shader_stage_requirements(const ShaderHandle& vert, const ShaderHandle& frag)
{
    ShaderStages stages{};
    stages.vsHash = std::make_optional(vert.acquire()->hash());
    stages.fsHash = std::make_optional(frag.acquire()->hash());

    return stages;
}
}    // namespace
namespace odin::graphics
{
ForwardPass::ForwardPass(registry::pipeline::PipelineRegistry& pipelineRegistry, registry::resource::ResourceRegistry& resourceRegistry)
    : m_PipelineRequest{}
    , m_Pipeline{ nullptr }
{
    ShaderHandle vert = resourceRegistry.shader("forward_pass.vert");
    ShaderHandle frag = resourceRegistry.shader("forward_pass.frag");

    ShaderStages stages = make_shader_stage_requirements(vert, frag);


    using DescriptorType = registry::pipeline::DescriptorType;
    using ShaderStage = registry::pipeline::ShaderStage;
    registry::pipeline::RequestBuilder builder{};
    builder.add_vertex_shader(std::move(vert))
        .add_fragment_shader(std::move(frag))
        .add_descriptor_layout(GLOBAL_SET_ID,
                               GLOBAL_SET_BIND_ID_CAMERA_DATA,
                               DescriptorType::dynamicUniformBuffer,
                               false,
                               ShaderStage::vertex,
                               ShaderStage::fragment)
        .add_color_format(VK_FORMAT_R8G8B8A8_SRGB)    // get this from swapchain's color attachment
        .add_polygon_mode(VK_POLYGON_MODE_FILL);


    m_PipelineRequest = builder.build();
    m_Pipeline = pipelineRegistry.graphics_pipeline(m_PipelineRequest);
}
}    // namespace odin::graphics
