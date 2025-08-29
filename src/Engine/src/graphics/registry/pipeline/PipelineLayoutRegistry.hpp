//
// Created by qwerty on 27/07/2025.
//
#pragma once

// std
#include <mutex>

#include "PipelineLayoutKey.hpp"
#include "descriptors/DescriptorSetLayoutRegistry.hpp"
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
    [[nodiscard]] PipelineLayoutKey make_layout_key(const Request& request);
    [[nodiscard]] PipelineLayoutRef pipeline_layout(const PipelineLayoutKey& key);
    [[nodiscard]] std::vector<vk::pipeline::DescriptorSetLayoutRef> descriptor_set_layouts(const PipelineLayoutKey& key);
    [[nodiscard]] bool update_after_bind(const PipelineLayoutKey& key, std::uint32_t setID) const;
private:
    void add_pipeline_layout(const PipelineLayoutKey& pipelineLayoutKey, std::span<const descriptors::DescriptorSetLayoutKey> descKeys);
private:
    vk::DeviceRef m_Device;
    descriptors::DescriptorSetLayoutRegistry m_DescriptorLayouts;
    Registry m_Layouts;
    std::unique_ptr<std::mutex> m_pMutex;
};
}    // namespace odin::graphics::registry::pipeline
