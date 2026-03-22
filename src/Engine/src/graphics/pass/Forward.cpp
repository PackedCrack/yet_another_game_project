//
// Created by qwerty on 26/07/2025.
//
#include "Forward.hpp"

#include <glm_headers.hpp>

#include "../gpu_types.hpp"
#include "../DepthAttachment.hpp"
#include "../registry/pipeline/Request.hpp"
#include "../registry/pipeline/RequestBuilder.hpp"
#include "../registry/resource/shader/ShaderHandle.hpp"
#include "../vk/pipeline/DescriptorWriter.hpp"
#include "../vk/ext/device/extended_dynamic_state3.hpp"
//
//
namespace
{
using namespace odin::graphics;
using namespace odin::graphics::pass;
//
//
[[nodiscard]] registry::pipeline::Request make_request(const Presenter& presenter, registry::resource::ResourceRegistry& resourceRegistry)
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
    builder.add_color_format(presenter.color_format()).add_polygon_mode(VK_POLYGON_MODE_FILL);

    // Cppcheck thinks we're using using vert/frag after move if these calls are chained..
    builder.add_vertex_shader(std::move(vert));
    builder.add_fragment_shader(std::move(frag));

    return builder.build();
}
[[nodiscard]] VkRenderingAttachmentInfo
make_attachment_info(vk::resource::ImageViewRef view, VkImageLayout layout, const VkClearValue& clear)
{
    VkRenderingAttachmentInfo info{};
    info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    info.pNext = nullptr;
    info.imageView = view.handle;
    info.imageLayout = layout;
    info.resolveMode = VK_RESOLVE_MODE_NONE;
    info.resolveImageView = VK_NULL_HANDLE;
    info.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;    // or LOAD if you preserved previous
    info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    info.clearValue = clear;

    return info;
}
[[nodiscard]] VkRenderingInfo
make_rendering_info(const VkRect2D& renderArea, const VkRenderingAttachmentInfo& colorInfo, const VkRenderingAttachmentInfo& depthInfo)
{
    VkRenderingInfo info{};
    info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.renderArea = renderArea;
    info.layerCount = 1;
    info.viewMask = 0;
    info.colorAttachmentCount = 1;
    info.pColorAttachments = std::addressof(colorInfo);
    info.pDepthAttachment = std::addressof(depthInfo);
    info.pStencilAttachment = nullptr;

    return info;
}
void begin_rendering(vk::CommandBufferRef cmd, const ColorAttachment& color, const DepthAttachment& depth)
{
    VkClearValue clearColor = { .color = { { 1.0f, 0.0f, 1.0f, 1.0f } } };
    VkRenderingAttachmentInfo colorInfo = make_attachment_info(color.view(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, clearColor);
    VkClearValue clearDepth = {
        .depthStencil = { 1.0f, 0 }
    };
    VkRenderingAttachmentInfo depthInfo = make_attachment_info(depth.view(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, clearDepth);

    VkRect2D renderArea{};
    renderArea.offset = { 0, 0 };
    renderArea.extent = color.extent();

    VkRenderingInfo info = make_rendering_info(renderArea, colorInfo, depthInfo);
    vkCmdBeginRendering(cmd.handle, std::addressof(info));
}
void set_dynamic_state(vk::CommandBufferRef cmd, const ColorAttachment& colorAttachment)
{
    vkCmdSetCullMode(cmd.handle, VK_CULL_MODE_BACK_BIT);

    vkCmdSetFrontFace(cmd.handle, VK_FRONT_FACE_CLOCKWISE);

    vkCmdSetDepthTestEnable(cmd.handle, VK_TRUE);
    vkCmdSetDepthWriteEnable(cmd.handle, VK_TRUE);
    vkCmdSetDepthCompareOp(cmd.handle, VK_COMPARE_OP_LESS);
    vkCmdSetStencilTestEnable(cmd.handle, VK_FALSE);

    VkBool32 enable = VK_FALSE;
    vk::ext::device::vkCmdSetColorBlendEnable(cmd.handle, 0, 1, std::addressof(enable));
    VkColorBlendEquationEXT equation{};
    vk::ext::device::vkCmdSetColorBlendEquation(cmd.handle, 0, 1, std::addressof(equation));

    VkViewport vp{};
    vp.width = static_cast<float>(colorAttachment.extent().width);
    vp.height = static_cast<float>(colorAttachment.extent().height);
    vp.maxDepth = 1.0f;
    vp.minDepth = 0.0f;
    vkCmdSetViewportWithCount(cmd.handle, 1, std::addressof(vp));
    VkRect2D scissor{};
    scissor.extent.width = static_cast<std::uint32_t>(vp.width);
    scissor.extent.height = static_cast<std::uint32_t>(vp.height);
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    vkCmdSetScissorWithCount(cmd.handle, 1, std::addressof(scissor));

    vkCmdSetPrimitiveTopology(cmd.handle, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

    vkCmdSetPrimitiveRestartEnable(cmd.handle, VK_FALSE);
}
}    // namespace
namespace odin::graphics::pass
{
Forward::Forward(const Presenter& presenter,
                 registry::pipeline::PipelineRegistry& pipelineRegistry,
                 registry::resource::ResourceRegistry& resourceRegistry)
    : registry::pipeline::PipelineResolver<Forward>{ pipelineRegistry }
    , m_GraphicsRequest{ make_request(presenter, resourceRegistry) }
    , m_Pipeline{ pipelineRegistry.graphics_pipeline(m_GraphicsRequest) }
{}
void Forward::execute(const FrameContext& frameContext,
                      const ColorAttachment& colorAttachment,
                      const descriptors::Global& global,
                      const descriptors::Indirect& indirect,
                      registry::resource::ResourceRegistry& resourceRegistry) const
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

    DepthAttachment depthAttachment = resourceRegistry.depth_attachment(frameContext, colorAttachment);
    begin_rendering(cmdBuffer, colorAttachment, depthAttachment);

    m_Pipeline.acquire()->pipeline.bind(cmdBuffer);

    bind_descriptors(frameContext, global, indirect);

    set_dynamic_state(cmdBuffer, colorAttachment);


    // Upload camera data here temporarily
    glm::vec3 position{ 0.0f, 0.0f, 3.0f };
    glm::vec3 target{ 0.0f, 0.0f, 0.0f };
    glm::vec3 up{ 0.0f, 1.0f, 0.0f };
    float fovY_deg = 60.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    float aspect = 1600.0f / 900.0f;
    std::array<CameraInfo, 1> cameraInfo{};
    cameraInfo[0].view = glm::lookAt(position, target, up);
    cameraInfo[0].proj = glm::perspective(glm::radians(fovY_deg), aspect, nearPlane, farPlane);
    cameraInfo[0].proj[1][1] *= -1.0f;
    cameraInfo[0].viewproj = cameraInfo[0].proj * cameraInfo[0].view;

    auto camBuf = resourceRegistry.buffer_registry().camera_data();
    camBuf->write<CameraInfo>(cameraInfo, frameContext.frame);


    auto drawCommands = indirect.view_draw_commands(frameContext.frame);
    auto drawVariables = indirect.view_draw_variables(frameContext.frame);
    auto maxDraws = static_cast<std::uint32_t>(drawCommands.range / sizeof(VkDrawIndexedIndirectCommand));
    vkCmdDrawIndexedIndirectCount(cmdBuffer.handle,
                                  drawCommands.handle,
                                  drawCommands.offset,
                                  drawVariables.handle,
                                  drawVariables.offset,
                                  maxDraws,
                                  sizeof(VkDrawIndexedIndirectCommand));

    vkCmdEndRendering(cmdBuffer.handle);
}
void Forward::bind_descriptors(const FrameContext& frameContext,
                               const descriptors::Global& global,
                               const descriptors::Indirect& indirect) const
{
    vk::pipeline::PipelineLayoutRef layout = resolve_layout(m_Pipeline);

    vk::CommandBufferRef cmdBuffer = frameContext.graphicsBuffer.get().handle();
    global.bind(cmdBuffer, layout, VK_SHADER_STAGE_ALL_GRAPHICS);
    indirect.bind(cmdBuffer, layout, VK_SHADER_STAGE_ALL_GRAPHICS, frameContext.frame);
}
}    // namespace odin::graphics::pass
