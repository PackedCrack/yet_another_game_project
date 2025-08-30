//
// Created by qwerty on 26/08/2025.
//
#pragma once

#include "GraphicsSlot.hpp"
#include "GraphicsHandle.hpp"
#include "PipelineKey.hpp"
#include "PipelineLayoutKey.hpp"
#include "PipelineLayoutRegistry.hpp"
#include "Request.hpp"
#include "../../vk/Device.hpp"
#include "../../vk/pipeline/DescriptorSetAllocator.hpp"
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
    using GraphicsRegistry = std::unordered_map<PipelineKey, std::weak_ptr<GraphicsSlot>, PipelineKeyHasher>;
public:
    [[nodiscard]] static std::unique_ptr<PipelineRegistry> make(vk::DeviceRef device);
private:
    PipelineRegistry(vk::DeviceRef device);
public:
    [[nodiscard]] GraphicsHandle graphics_pipeline(const Request& request);
private:
    [[nodiscard]] std::shared_ptr<GraphicsSlot> make_graphics_slot(const PipelineKey& key, const Request& request);
    [[nodiscard]] std::shared_ptr<GraphicsResource> make_graphics_resource(const Request& request);
    [[nodiscard]] GraphicsHandle make_graphics_handle(std::shared_ptr<GraphicsSlot> pSlot);
    [[nodiscard]] std::shared_ptr<GraphicsSlot> get_graphics_pipeline_slot(const PipelineKey& key, const Request& request);
    [[nodiscard]] VkDescriptorSet allocate_descriptor_set(GraphicsHandle handle, std::uint32_t setID);
    [[nodiscard]] PipelineKey make_pipeline_key(const Request& request);
    void rebuild_graphics_pipeline(std::shared_ptr<GraphicsSlot>& pSlot);
private:
    vk::DeviceRef m_Device;
    PipelineLayoutRegistry m_PipelineLayouts;
    vk::pipeline::DescriptorSetAllocator m_DescriptorAllocator;
    GraphicsRegistry m_GraphicsPipelines;
    Mutex m_pMutex;
};
}    // namespace odin::graphics::registry::pipeline
