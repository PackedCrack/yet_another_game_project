//
// Created by qwerty on 26/07/2025.
//
#include "ForwardPass.hpp"

#include "gpu_types.hpp"
#include "registry/pipeline/Request.hpp"
#include "registry/pipeline/RequestBuilder.hpp"
#include "registry/resource/shader/ShaderHandle.hpp"
//
//
namespace
{
using namespace odin::graphics;
//
//
[[nodiscard]] registry::pipeline::Request make_request(registry::resource::ResourceRegistry& resourceRegistry)
{
    using ShaderStage = registry::pipeline::ShaderStage;
    using ShaderHandle = registry::resource::shader::ShaderHandle;
    using DescriptorType = registry::pipeline::DescriptorType;
    using ShaderStage = registry::pipeline::ShaderStage;


    ShaderHandle vert = resourceRegistry.shader("forward_pass.vert");
    ShaderHandle frag = resourceRegistry.shader("forward_pass.frag");

    registry::pipeline::RequestBuilder builder{};
    builder
        .add_descriptor_layout(GLOBAL_SET_ID,
                               GLOBAL_SET_BIND_ID_CAMERA_DATA,
                               DescriptorType::dynamicUniformBuffer,
                               false,
                               ShaderStage::vertex,
                               ShaderStage::fragment)
        .add_color_format(VK_FORMAT_R8G8B8A8_SRGB)    // get this from swapchain's color attachment
        .add_polygon_mode(VK_POLYGON_MODE_FILL);

    // Cppcheck thinks we're using using vert/frag after move if these calls are chained..
    builder.add_vertex_shader(std::move(vert));
    builder.add_fragment_shader(std::move(frag));

    return builder.build();
}
}    // namespace
namespace odin::graphics
{
ForwardPass::ForwardPass(registry::pipeline::PipelineRegistry& pipelineRegistry, registry::resource::ResourceRegistry& resourceRegistry)
    : m_GraphicsRequest{ make_request(resourceRegistry) }
    , m_Pipeline{ pipelineRegistry.graphics_pipeline(m_GraphicsRequest) }
{
    VkDescriptorSet set = pipelineRegistry.allocate_descriptor_set(m_Pipeline, GLOBAL_SET_ID);
}
}    // namespace odin::graphics
