//
// Created by qwerty on 30/08/2025.
//
#pragma once

#include "../Device.hpp"
#include "../resource/DynamicUniformBuffer.hpp"
#include "../resource/DynamicStorageBuffer.hpp"
#include "../resource/StorageBuffer.hpp"
//
//
namespace odin::graphics::vk::pipeline
{
class DescriptorWriter
{
public:
    DescriptorWriter(DeviceRef device, VkDescriptorSet set);
public:
    DescriptorWriter& add_dynamic_uniform_buffer(std::uint32_t bindingID, const resource::DynamicUniformBuffer& buffer);
    DescriptorWriter& add_dynamic_storage_buffer(std::uint32_t bindingID, const resource::DynamicStorageBuffer& buffer);
    DescriptorWriter& add_storage_buffer(std::uint32_t bindingID, const resource::StorageBuffer& buffer);
    void write_descriptor_set();
private:
    DescriptorWriter& add_buffer(std::uint32_t bindingID, VkDescriptorType type, VkBuffer buffer, VkDeviceSize range);
    void invalidate();
private:
    DeviceRef m_Device;
    VkDescriptorSet m_Set;
    std::vector<VkWriteDescriptorSet> m_Writes;
    std::list<VkDescriptorBufferInfo> m_BufferInfo;
};
}    // namespace odin::graphics::vk::pipeline
