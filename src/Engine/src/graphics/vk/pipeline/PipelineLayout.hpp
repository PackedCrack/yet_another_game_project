//
// Created by qwerty on 17/08/2025.
//
#pragma once

#include "DescriptorSetLayout.hpp"
#include "../Device.hpp"
// vulkan
#include <vulkan/vulkan.h>
//
//
namespace odin::graphics::vk::pipeline
{
struct PipelineLayoutRef
{
    VkPipelineLayout handle;
};
class PipelineLayout
{
public:
    PipelineLayout(DeviceRef device,
                   std::span<const DescriptorSetLayoutRef> descriptorLayouts,
                   std::span<VkPushConstantRange> pushContantRanges);
    ~PipelineLayout();
    PipelineLayout(const PipelineLayout& other) = delete;
    PipelineLayout(PipelineLayout&& other) noexcept;
    PipelineLayout& operator=(const PipelineLayout& other) = delete;
    PipelineLayout& operator=(PipelineLayout&& other) noexcept;
public:
    [[nodiscard]] PipelineLayoutRef handle() const;
private:
    VkPipelineLayout m_Layout = VK_NULL_HANDLE;
    DeviceRef m_Device;
};
}    // namespace odin::graphics::vk::pipeline
