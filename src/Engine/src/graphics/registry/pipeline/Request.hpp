//
// Created by qwerty on 27/07/2025.
//
#pragma once

#include "../resource/ShaderHandle.hpp"
// vulkan
#include <vulkan/vulkan.h>
//
//
namespace odin::graphics::registry::pipeline
{
struct ShaderStages
{
    std::optional<resource::ShaderHandle> cs;
    std::optional<resource::ShaderHandle> vs;
    std::optional<resource::ShaderHandle> fs;
};
struct DescriptorRequest
{
    std::uint32_t bindingID;
    VkDescriptorType type;
    std::uint32_t numDescriptors;
    VkShaderStageFlags stages;
    VkDescriptorBindingFlags flags;
};
struct Request
{
    ShaderStages stages;
    std::vector<std::vector<DescriptorRequest>> DescLayoutRequirement;

    std::optional<std::vector<VkFormat>> colorFormats;
    std::optional<VkFormat> depthFormat;
    std::optional<VkFormat> stencilFormat;

    std::optional<VkPolygonMode> polygonMode;
    std::optional<VkSampleCountFlagBits> MsaaSamples;
};
}    // namespace odin::graphics::registry::pipeline
