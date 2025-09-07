//
// Created by qwerty on 26/07/2025.
//
#include "Forward.hpp"

#include "../gpu_types.hpp"
#include "../registry/pipeline/Request.hpp"
#include "../registry/pipeline/RequestBuilder.hpp"
#include "../registry/resource/shader/ShaderHandle.hpp"
#include "../vk/pipeline/DescriptorWriter.hpp"
//
//
namespace
{
using namespace odin::graphics;
using namespace odin::graphics::pass;
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
    builder = descriptors::global_preset(builder);
    builder = descriptors::indirect_preset(builder);
    builder
        .add_color_format(VK_FORMAT_R8G8B8A8_SRGB)    // get this from swapchain's color attachment
        .add_polygon_mode(VK_POLYGON_MODE_FILL);

    // Cppcheck thinks we're using using vert/frag after move if these calls are chained..
    builder.add_vertex_shader(std::move(vert));
    builder.add_fragment_shader(std::move(frag));

    return builder.build();
}
[[nodiscard]] VkRenderingInfo make_rendering_info(const ColorAttachment& colorAttachment)
{
    vk::resource::ImageViewRef colorView = colorAttachment.view();

    VkRenderingAttachmentInfo colorAtt{};
    colorAtt.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAtt.pNext = nullptr;
    colorAtt.imageView = colorView.handle;
    colorAtt.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAtt.resolveMode = VK_RESOLVE_MODE_NONE;
    colorAtt.resolveImageView = VK_NULL_HANDLE;
    colorAtt.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAtt.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,    // or LOAD if you preserved previou;
        colorAtt.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAtt.clearValue = { .color = { { 1.0f, 0.0f, 1.0f, 1.0f } } };

    VkRenderingInfo info{};
    info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.renderArea = {
        { 0, 0 },
        colorAttachment.extent()
    };
    info.layerCount = 1;
    info.viewMask = 0;
    info.colorAttachmentCount = 1;
    info.pColorAttachments = &colorAtt;
    info.pDepthAttachment = nullptr;
    info.pStencilAttachment = nullptr;

    return info;
}
}    // namespace
namespace odin::graphics::pass
{
Forward::Forward(registry::pipeline::PipelineRegistry& pipelineRegistry, registry::resource::ResourceRegistry& resourceRegistry)
    : m_GraphicsRequest{ make_request(resourceRegistry) }
    , m_Pipeline{ pipelineRegistry.graphics_pipeline(m_GraphicsRequest) }
    , m_PipelineRegistry{ pipelineRegistry }
{}
void Forward::execute(const FrameContext& frameContext,
                      const ColorAttachment& colorAttachment,
                      const descriptors::Global& global,
                      const descriptors::Indirect& indirect) const
{
    vk::CommandBufferRef cmdBuffer = frameContext.graphicsBuffer.get().handle();

    VkImageMemoryBarrier2 renderBarrier = colorAttachment.barrier_to_render();
    const VkDependencyInfo dep{ .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                                .pNext = nullptr,
                                .dependencyFlags = 0,
                                .memoryBarrierCount = 0,
                                .pMemoryBarriers = nullptr,
                                .bufferMemoryBarrierCount = 0,
                                .pBufferMemoryBarriers = nullptr,
                                .imageMemoryBarrierCount = 1,
                                .pImageMemoryBarriers = std::addressof(renderBarrier) };
    vkCmdPipelineBarrier2(cmdBuffer.handle, std::addressof(dep));

    VkRenderingInfo info = make_rendering_info(colorAttachment);
    vkCmdBeginRendering(cmdBuffer.handle, std::addressof(info));

    m_Pipeline.acquire()->pipeline.bind(cmdBuffer);

    bind_descriptors(frameContext, global, indirect);


    vkCmdEndRendering(cmdBuffer.handle);
}
void Forward::bind_descriptors(const FrameContext& frameContext,
                               const descriptors::Global& global,
                               const descriptors::Indirect& indirect) const
{
    const registry::pipeline::PipelineLayoutKey& key = m_Pipeline.acquire()->pipelineLayoutKey;
    vk::pipeline::PipelineLayoutRef layout = m_PipelineRegistry.get().pipeline_layout(key);

    vk::CommandBufferRef cmdBuffer = frameContext.graphicsBuffer.get().handle();
    global.bind(cmdBuffer, layout, VK_SHADER_STAGE_ALL_GRAPHICS);
    indirect.bind(cmdBuffer, layout, VK_SHADER_STAGE_ALL_GRAPHICS, frameContext.frame);
}
}    // namespace odin::graphics::pass
