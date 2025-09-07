//
// Created by qwerty on 30/08/2025.
//
#include "DescriptorWriter.hpp"
//
//
namespace
{
using BindType =  odin::graphics::registry::resource::buffer::BindType;
//
//
[[nodiscard]] VkDescriptorBufferInfo make_buffer_info(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range)
{
    VkDescriptorBufferInfo info{};
    info.buffer = buffer;
    info.offset = offset;
    info.range = range;

    return info;
}
// clang-format off
[[nodiscard]] VkWriteDescriptorSet make_write_descriptor_set(
    VkDescriptorSet set, 
    std::uint32_t bindingID, 
    std::uint32_t firstElement,
    BindType type,
    std::uint32_t count)
{
    VkWriteDescriptorSet writeSet{};
    writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeSet.pNext = nullptr;
    writeSet.dstSet = set;
    writeSet.dstBinding = bindingID;
    writeSet.dstArrayElement = firstElement;
    writeSet.descriptorCount = count;
    writeSet.pImageInfo = nullptr;
    writeSet.pBufferInfo = nullptr;
    writeSet.pTexelBufferView = nullptr;

    switch (type)
    {
    case BindType::storage:
        writeSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        break;
    case BindType::dynamicStorage:
        writeSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        break;
    case BindType::dynamicUniform:
        writeSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        break;
    }

    return writeSet;
}
// clang-format on
}    // namespace
namespace odin::graphics::vk::pipeline
{
DescriptorWriter::DescriptorWriter(DeviceRef device, VkDescriptorSet set)
    : m_Device{ device }
    , m_Set{ set }
{}
DescriptorWriter& DescriptorWriter::add_buffer(std::uint32_t bindingID, const registry::resource::buffer::BindView& view)
{
    m_BufferInfo.emplace_back(make_buffer_info(view.handle, view.offset, view.range));
    m_Writes.emplace_back(make_write_descriptor_set(m_Set, bindingID, 0, view.type, 1));
    m_Writes.back().pBufferInfo = std::addressof(m_BufferInfo.back());

    return *this;
}
void DescriptorWriter::write_descriptor_set()
{
    ODIN_ASSERT(m_Set != VK_NULL_HANDLE);
    vkUpdateDescriptorSets(m_Device.handle, static_cast<std::uint32_t>(m_Writes.size()), m_Writes.data(), 0, nullptr);
    invalidate();
}
void DescriptorWriter::invalidate()
{
    m_Device.handle = VK_NULL_HANDLE;
    m_Set = VK_NULL_HANDLE;
    m_BufferInfo.clear();
    m_Writes.clear();
}
}    // namespace odin::graphics::vk::pipeline
