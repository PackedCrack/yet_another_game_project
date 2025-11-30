//
// Created by qwerty on 30/08/2025.
//
#pragma once

#include "../Device.hpp"
#include "../resource/DynamicUniformBuffer.hpp"
#include "../resource/DynamicStorageBuffer.hpp"
#include "../resource/StorageBuffer.hpp"
#include "../../registry/resource/buffer/BindView.hpp"
//
//
namespace odin::graphics::vk::pipeline
{
class DescriptorWriter
{
public:
    DescriptorWriter(DeviceRef device, VkDescriptorSet set);
public:
    DescriptorWriter& add_buffer(std::uint32_t bindingID, const registry::resource::buffer::BindView& view);
    void write_descriptor_set();
private:
    void invalidate();
private:
    DeviceRef m_Device;
    VkDescriptorSet m_Set;
    std::vector<VkWriteDescriptorSet> m_Writes;
    std::list<VkDescriptorBufferInfo> m_BufferInfo;
};
}    // namespace odin::graphics::vk::pipeline
