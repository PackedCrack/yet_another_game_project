//
// Created by qwerty on 27/08/2025.
//
#pragma once

// std
#include <mutex>

#include "PipelineLayoutKey.hpp"
#include "DescriptorSetLayoutRegistry.hpp"
#include "../../vk/Device.hpp"
#include "../../vk/pipeline/PipelineLayout.hpp"
//
//
namespace odin::graphics::registry::pipeline
{
class PipelineLayoutRegistry
{
    using PipelineLayout = vk::pipeline::PipelineLayout;
    using PipelineLayoutRef = vk::pipeline::PipelineLayoutRef;
    using Registry = std::unordered_map<PipelineLayoutKey, PipelineLayout, PipelineLayoutKeyHasher>;
public:
    PipelineLayoutRegistry(vk::DeviceRef device);
public:
    [[nodiscard]] PipelineLayoutKey make_layout_key(const Request& request, DescriptorSetLayoutRegistry& descriptorLayoutRegistry);
    [[nodiscard]] PipelineLayoutRef pipeline_layout(const PipelineLayoutKey& key) const;
private:
    void add_pipeline_layout(const PipelineLayoutKey& pipelineLayoutKey,
                             DescriptorSetLayoutRegistry& descriptorLayoutRegistry,
                             std::span<const DescriptorSetLayoutKey> descKeys);
private:
    vk::DeviceRef m_Device;
    Registry m_Layouts;
    std::unique_ptr<std::mutex> m_pMutex;
};
}    // namespace odin::graphics::registry::pipeline
