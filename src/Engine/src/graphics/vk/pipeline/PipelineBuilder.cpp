//
// Created by qwerty on 26/08/2025.
//

#include "PipelineBuilder.hpp"
// common
#include <common.hpp>
//
//
namespace
{
using namespace odin::graphics::vk;
using namespace odin::graphics::vk::pipeline;

constexpr std::array<VkDynamicState, 15> enabledDynamicStates{ VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT,
                                                               VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT,
                                                               VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY,
                                                               VK_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE,
                                                               VK_DYNAMIC_STATE_CULL_MODE,
                                                               VK_DYNAMIC_STATE_FRONT_FACE,
                                                               VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE,
                                                               VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE,
                                                               VK_DYNAMIC_STATE_DEPTH_COMPARE_OP,
                                                               VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE,
                                                               VK_DYNAMIC_STATE_STENCIL_WRITE_MASK,
                                                               VK_DYNAMIC_STATE_STENCIL_OP,
                                                               VK_DYNAMIC_STATE_COLOR_BLEND_ENABLE_EXT,
                                                               VK_DYNAMIC_STATE_COLOR_BLEND_EQUATION_EXT,
                                                               VK_DYNAMIC_STATE_BLEND_CONSTANTS };

constexpr std::uint16_t SHADER_MODULES_BIT = 1 << 0;
constexpr std::uint16_t VERTEX_INPUT_BIT = 1 << 1;
constexpr std::uint16_t RASTERIZATION_BIT = 1 << 2;
constexpr std::uint16_t MULTISAMPLING_BIT = 1 << 3;
constexpr std::uint16_t DYNAMIC_RENDERING_BIT = 1 << 4;
constexpr std::uint16_t GRAPHICS_DATA_FULLFILLED = SHADER_MODULES_BIT | RASTERIZATION_BIT | MULTISAMPLING_BIT | DYNAMIC_RENDERING_BIT;
//
//
[[nodiscard]] VkPipelineShaderStageCreateInfo make_shader_stage_create_info(resource::ShaderModuleRef shader, VkShaderStageFlagBits stage)
{
    VkPipelineShaderStageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = VK_NO_FLAGS;
    info.stage = stage;
    info.module = shader.handle;
    info.pName = "main";
    info.pSpecializationInfo = nullptr;

    return info;
}
// clang-format off
[[nodiscard]] VkPipelineVertexInputStateCreateInfo make_vertex_input_state_create_info(
    std::span<VkVertexInputBindingDescription> bindingDescriptions, 
    std::span<VkVertexInputAttributeDescription> attributeDescriptions)
{
    VkPipelineVertexInputStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = VK_NO_FLAGS;
    info.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
    info.pVertexBindingDescriptions = bindingDescriptions.empty() ? nullptr : bindingDescriptions.data();
    info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    info.pVertexAttributeDescriptions = attributeDescriptions.empty() ? nullptr : attributeDescriptions.data();

    return info;
}
// clang-format on
[[nodiscard]] VkPipelineDepthStencilStateCreateInfo make_depth_stencil_create_info()
{
    VkPipelineDepthStencilStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.depthTestEnable = VK_FALSE;
    info.depthWriteEnable = VK_FALSE;
    info.depthCompareOp = VK_COMPARE_OP_NEVER;
    info.depthBoundsTestEnable = VK_FALSE;
    info.stencilTestEnable = VK_FALSE;

    VkStencilOpState front{};
    front.failOp = VK_STENCIL_OP_KEEP;
    front.passOp = VK_STENCIL_OP_KEEP;
    front.depthFailOp = VK_STENCIL_OP_KEEP;
    front.compareOp = VK_COMPARE_OP_ALWAYS;
    info.front = front;

    VkStencilOpState back{};
    back.failOp = VK_STENCIL_OP_KEEP;
    back.passOp = VK_STENCIL_OP_KEEP;
    back.depthFailOp = VK_STENCIL_OP_KEEP;
    back.compareOp = VK_COMPARE_OP_ALWAYS;

    info.back = back;
    info.minDepthBounds = 0.0f;    // Optional
    info.maxDepthBounds = 1.0f;    // Optional

    return info;
}
[[nodiscard]] VkPipelineRasterizationStateCreateInfo make_rasterization_create_info(VkPolygonMode polygonMode, bool depthBias)
{
    VkPipelineRasterizationStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = NULL;
    info.depthClampEnable = VK_FALSE;
    info.rasterizerDiscardEnable = VK_FALSE;
    info.polygonMode = polygonMode;
    info.cullMode = VK_CULL_MODE_NONE;
    info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    info.depthBiasEnable = depthBias;
    info.depthBiasConstantFactor = 0.0f;
    info.depthBiasClamp = 0.0f;
    info.depthBiasSlopeFactor = 0.0f;
    info.lineWidth = 1.0f;

    return info;
}
[[nodiscard]] VkPipelineMultisampleStateCreateInfo make_multisampling_create_info(VkSampleCountFlagBits sampleCount)
{
    VkPipelineMultisampleStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = VK_NO_FLAGS;
    info.rasterizationSamples = sampleCount;
    info.sampleShadingEnable = VK_FALSE;
    info.minSampleShading = 1.0f;
    info.pSampleMask = nullptr;
    info.alphaToCoverageEnable = VK_FALSE;
    info.alphaToOneEnable = VK_FALSE;

    return info;
}
[[nodiscard]] VkPipelineColorBlendAttachmentState make_color_blend_attachment_state()
{
    VkPipelineColorBlendAttachmentState state{};
    state.blendEnable = VK_FALSE;
    state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    state.colorBlendOp = VK_BLEND_OP_ADD;
    state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    state.alphaBlendOp = VK_BLEND_OP_ADD;
    state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    return state;
}
[[nodiscard]] VkPipelineColorBlendStateCreateInfo make_color_blend_create_info(std::span<VkPipelineColorBlendAttachmentState> colorblend)
{
    VkPipelineColorBlendStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = VK_NO_FLAGS;
    info.logicOpEnable = VK_FALSE;
    info.logicOp = VK_LOGIC_OP_COPY;
    info.attachmentCount = static_cast<uint32_t>(colorblend.size());
    info.pAttachments = colorblend.empty() ? nullptr : colorblend.data();

    return info;
}
[[nodiscard]] VkPipelineRenderingCreateInfo
make_rendering_info(std::span<const VkFormat> colorFormats, std::optional<VkFormat> depthFormat, std::optional<VkFormat> stencilFormat)
{
    VkPipelineRenderingCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    info.pNext = nullptr;
    info.viewMask = 0;    // No multiview support
    info.colorAttachmentCount = static_cast<std::uint32_t>(colorFormats.size());
    info.pColorAttachmentFormats = colorFormats.empty() ? nullptr : colorFormats.data();
    info.depthAttachmentFormat = depthFormat.value_or(VK_FORMAT_UNDEFINED);
    info.stencilAttachmentFormat = stencilFormat.value_or(VK_FORMAT_UNDEFINED);

    return info;
}
}    // namespace
namespace odin::graphics::vk::pipeline
{
PipelineBuilder::PipelineBuilder(DeviceRef device)
    : m_Device{ device }
    , m_GraphicsMask{}
    , m_Viewport{}
    , m_Scissor{}
    , m_DynamicStates{ std::begin(enabledDynamicStates), std::end(enabledDynamicStates) }
    , m_DynamicState{}
    , m_ShaderModules{}
    , m_ShaderStages{}
    , m_VertexInputState{}
    , m_DepthStencilState{ make_depth_stencil_create_info() }
    , m_RasterizationState{}
    , m_MultisamplingState{}
    , m_ColorBlendAttachments{}
    , m_ColorBlendState{}
    , m_RenderingInfo{}
{
    add_dynamic_states();
}
PipelineBuilder& PipelineBuilder::shader_module(const registry::resource::ShaderHandle& shader, VkShaderStageFlagBits stage)
{
    resource::ShaderModuleRef shaderRef = shader.acquire()->handle();
    m_ShaderModules.emplace_back(shaderRef);
    m_ShaderStages.emplace_back(make_shader_stage_create_info(shaderRef, stage));
    m_GraphicsMask |= SHADER_MODULES_BIT;

    return *this;
}
PipelineBuilder& PipelineBuilder::vertex_input_state(std::span<VkVertexInputBindingDescription> vertexBindings,
                                                     std::span<VkVertexInputAttributeDescription> vertexAttributes)
{
    m_VertexInputState = make_vertex_input_state_create_info(vertexBindings, vertexAttributes);
    m_GraphicsMask |= VERTEX_INPUT_BIT;

    return *this;
}
PipelineBuilder& PipelineBuilder::rasterization_state(VkPolygonMode polygonMode, bool depthBias)
{
    m_RasterizationState = make_rasterization_create_info(polygonMode, depthBias);
    m_GraphicsMask |= RASTERIZATION_BIT;

    return *this;
}
PipelineBuilder& PipelineBuilder::multisampling_state(VkSampleCountFlagBits sampleCount)
{
    m_MultisamplingState = make_multisampling_create_info(sampleCount);
    m_GraphicsMask |= MULTISAMPLING_BIT;

    return *this;
}
PipelineBuilder& PipelineBuilder::dynamic_rendering(std::span<const VkFormat> colorFormats,
                                                    std::optional<VkFormat> depthFormat,
                                                    std::optional<VkFormat> stencilFormat)
{
    ODIN_ASSERT((m_GraphicsMask & DYNAMIC_RENDERING_BIT) == 0);

    m_RenderingInfo = make_rendering_info(colorFormats, std::move(depthFormat), std::move(stencilFormat));
    if (!colorFormats.empty())
    {
        for (std::uint32_t i = 0; i < m_RenderingInfo.colorAttachmentCount; ++i)
        {
            m_ColorBlendAttachments.emplace_back(make_color_blend_attachment_state());
        }
        m_ColorBlendState = make_color_blend_create_info(common::to_span(m_ColorBlendAttachments));
    }
    ODIN_ASSERT(m_RenderingInfo.colorAttachmentCount == m_ColorBlendState.attachmentCount);

    m_GraphicsMask |= DYNAMIC_RENDERING_BIT;

    return *this;
}
GraphicsPipeline PipelineBuilder::build_graphics_pipeline(PipelineLayoutRef layout)
{
    VkGraphicsPipelineCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    info.pNext = std::addressof(m_RenderingInfo);
    info.pVertexInputState = std::addressof(m_VertexInputState);
    info.stageCount = static_cast<uint32_t>(m_ShaderStages.size());
    info.pStages = m_ShaderStages.data();
    info.pDynamicState = std::addressof(m_DynamicState);
    info.pRasterizationState = std::addressof(m_RasterizationState);
    info.pMultisampleState = std::addressof(m_MultisamplingState);
    info.pDepthStencilState = std::addressof(m_DepthStencilState);
    info.layout = layout.handle;
    info.renderPass = VK_NULL_HANDLE;
    info.subpass = 0;
    info.basePipelineHandle = VK_NULL_HANDLE;

    // If there are color attachments in this pipeline
    info.pColorBlendState = (m_RenderingInfo.colorAttachmentCount == 0) ? nullptr : std::addressof(m_ColorBlendState);

    // Set through dynamic state
    info.pViewportState = nullptr;
    info.pInputAssemblyState = nullptr;


    ODIN_ASSERT(validate_graphics_pipeline_data());
    return GraphicsPipeline(m_Device, info);
}
void PipelineBuilder::add_dynamic_states()
{
    m_DynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    m_DynamicState.pNext = nullptr;
    m_DynamicState.flags = VK_NO_FLAGS;
    m_DynamicState.dynamicStateCount = static_cast<std::uint32_t>(m_DynamicStates.size());
    m_DynamicState.pDynamicStates = m_DynamicStates.data();
}
[[nodiscard]] bool PipelineBuilder::validate_graphics_pipeline_data() const
{
    auto it = std::find_if(std::begin(m_ShaderStages),
                           std::end(m_ShaderStages),
                           [](const VkPipelineShaderStageCreateInfo& info) { return info.stage == VK_SHADER_STAGE_VERTEX_BIT; });
    if (it != std::end(m_ShaderStages))
    {
        if ((m_GraphicsMask & VERTEX_INPUT_BIT) == 0)
        {
            return false;
        }
    }

    return m_GraphicsMask == GRAPHICS_DATA_FULLFILLED;
}
//[[nodiscard]] bool validate_compute_pipeline_data() const
//{
//
//}
}    // namespace odin::graphics::vk::pipeline
