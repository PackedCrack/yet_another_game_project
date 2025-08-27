//
// Created by qwerty on 26/07/2025.
//
#pragma once

#include "GraphicsSlot.hpp"
#include "GraphicsHandle.hpp"
#include "DescriptorSetLayoutKey.hpp"
#include "PipelineKey.hpp"
#include "PipelineLayoutKey.hpp"
#include "Request.hpp"
#include "../../vk/Device.hpp"
#include "../../vk/pipeline/DescriptorSetLayout.hpp"
#include "../../vk/pipeline/GraphicsPipeline.hpp"
#include "../../vk/pipeline/PipelineLayout.hpp"
//
//
namespace odin::graphics::registry::pipeline
{
class PipelineRegistry
{
    using mutex_t = std::mutex;
    using Mutex = std::unique_ptr<mutex_t>;
    using DescriptorSetLayoutCache = std::unordered_map<DescriptorSetLayoutKey, vk::pipeline::DescriptorSetLayout, DescriptorSetLayoutKeyHasher>;
    using PipelineLayoutCache = std::unordered_map<PipelineLayoutKey, vk::pipeline::PipelineLayout, PipelineLayoutKeyHasher>;
    using GraphicsResource = vk::pipeline::GraphicsPipeline;
    using GraphicsRegistry = std::unordered_map<PipelineKey, std::weak_ptr<GraphicsSlot>, PipelineKeyHasher>;
public:
    PipelineRegistry(vk::DeviceRef device);
public:
    [[nodiscard]] GraphicsHandle graphics_pipeline(const Request& request);
private:
    void add_descriptor_layout(const DescriptorSetLayoutKey& key);
    void add_pipeline_layout(const PipelineLayoutKey& pipelineLayoutKey, std::span<const DescriptorSetLayoutKey> keys);
    void add_graphics_pipeline(const PipelineKey& key, const PipelineLayoutKey& layoutKey, const Request& request);
    [[nodiscard]] std::shared_ptr<GraphicsSlot> graphics_pipeline_slot(PipelineKey key);
    [[nodiscard]] std::tuple<PipelineKey, PipelineLayoutKey> make_pipeline_key(const Request& request);
    [[nodiscard]] std::vector<DescriptorSetLayoutKey> make_descriptor_set_layout_key(const Request& request);
    [[nodiscard]] PipelineLayoutKey make_pipeline_layout_key(std::span<const DescriptorSetLayoutKey> descLayoutKeys);
private:
    vk::DeviceRef m_Device;
    DescriptorSetLayoutCache m_DescriptorSetLayouts;
    PipelineLayoutCache m_PipelineLayouts;
    GraphicsRegistry m_GraphicsPipelines;
    Mutex m_pMutex;
};
}    // namespace odin::graphics::registry::pipeline
