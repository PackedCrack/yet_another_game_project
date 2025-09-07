//
// Created by qwerty on 28/08/2025.
//
#pragma once

// std
#include <unordered_map>

#include "DescriptorSetLayoutKey.hpp"
#include "Request.hpp"
#include "../../vk/Device.hpp"
#include "../../vk/pipeline/DescriptorSetLayout.hpp"
//
//
namespace odin::graphics::registry::pipeline
{
class DescriptorSetLayoutRegistry
{
    using DescriptorSetLayout = vk::pipeline::DescriptorSetLayout;
    using DescriptorSetLayoutRef = vk::pipeline::DescriptorSetLayoutRef;
    using Registry = std::unordered_map<DescriptorSetLayoutKey, DescriptorSetLayout, DescriptorSetLayoutKeyHasher>;
public:
    DescriptorSetLayoutRegistry(vk::DeviceRef device);
public:
    [[nodiscard]] std::vector<DescriptorSetLayoutRef> descriptor_set_layouts(std::span<const DescriptorSetLayoutKey> keys);
    [[nodiscard]] DescriptorSetLayoutRef descriptor_set_layout(const DescriptorSetLayoutKey& key);
    [[nodiscard]] std::vector<DescriptorSetLayoutKey> make_layout_keys(const Request& request);
    [[nodiscard]] bool requires_update_after_bind(const DescriptorSetLayoutKey& key) const;
private:
    void add_descriptor_layout(const DescriptorSetLayoutKey& key);
private:
    vk::DeviceRef m_Device;
    Registry m_Layouts;
    std::unique_ptr<std::mutex> m_pMutex;
};
}    // namespace odin::graphics::registry::pipeline
