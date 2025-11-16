//
// Created by qwerty on 26/08/2025.
//
#pragma once

#include "GraphicsPipeline.hpp"
#include "ComputePipeline.hpp"
#include "PipelineLayout.hpp"
#include "../Device.hpp"
#include "../resource/ShaderModule.hpp"
#include "../../registry/resource/shader/ShaderHandle.hpp"
// vulkan
#include <vulkan/vulkan.h>
//
//
namespace odin::graphics::vk::pipeline
{
class PipelineBuilder
{
public:
    PipelineBuilder(DeviceRef device);
public:
    PipelineBuilder& shader_module(const registry::resource::shader::ShaderHandle& shader, VkShaderStageFlagBits stage);
    PipelineBuilder& add_vertex_module(const registry::resource::shader::ShaderHandle& shader);
    PipelineBuilder& add_fragment_module(const registry::resource::shader::ShaderHandle& shader);
    PipelineBuilder& add_compute_module(const registry::resource::shader::ShaderHandle& shader);
    PipelineBuilder& vertex_input_state(std::span<VkVertexInputBindingDescription> vertexBindings,
                                        std::span<VkVertexInputAttributeDescription> vertexAttributes);
    PipelineBuilder& rasterization_state(VkPolygonMode polygonMode, bool depthBias = false);
    PipelineBuilder& multisampling_state(VkSampleCountFlagBits sampleCount);
    PipelineBuilder&
    dynamic_rendering(std::span<const VkFormat> colorFormats, std::optional<VkFormat> depthFormat, std::optional<VkFormat> stencilFormat);
    GraphicsPipeline build_graphics_pipeline(PipelineLayoutRef layout);
    ComputePipeline build_compute_pipeline(PipelineLayoutRef layout);
private:
    void add_dynamic_states();
    [[nodiscard]] bool validate_graphics_pipeline_data() const;
    [[nodiscard]] bool validate_compute_pipeline_data() const;
private:
    DeviceRef m_Device;

    bool m_HasShaderStage;
    // Requirements for ComputePipeline goes here

    // Requirements for GraphicsPipeline
    std::uint16_t m_GraphicsMask;
    VkViewport m_Viewport;
    VkRect2D m_Scissor;
    std::vector<VkDynamicState> m_DynamicStates;
    VkPipelineDynamicStateCreateInfo m_DynamicState;
    // Should be ShadeModuleHandle from the ResourceRegistry in the future
    std::vector<resource::ShaderModuleRef> m_ShaderModules;
    std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
    VkPipelineVertexInputStateCreateInfo m_VertexInputState;
    VkPipelineDepthStencilStateCreateInfo m_DepthStencilState;
    VkPipelineRasterizationStateCreateInfo m_RasterizationState;
    VkPipelineMultisampleStateCreateInfo m_MultisamplingState;
    std::vector<VkPipelineColorBlendAttachmentState> m_ColorBlendAttachments;
    VkPipelineColorBlendStateCreateInfo m_ColorBlendState;
    VkPipelineRenderingCreateInfo m_RenderingInfo;
};
}    // namespace odin::graphics::vk::pipeline
