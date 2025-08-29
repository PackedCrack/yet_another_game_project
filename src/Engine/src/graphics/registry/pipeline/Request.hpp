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
    std::optional<VkPolygonMode> polygonMode;

    std::optional<std::vector<VkFormat>> colorFormats;
    std::optional<VkFormat> depthFormat;
    std::optional<VkFormat> stencilFormat;

    std::optional<VkSampleCountFlagBits> MsaaSamples;

    std::vector<std::vector<DescriptorRequest>> DescLayoutRequirement;

    std::optional<resource::ShaderHandle> vs;
    std::optional<resource::ShaderHandle> fs;
    std::optional<resource::ShaderHandle> cs;
};
}    // namespace odin::graphics::registry::pipeline
