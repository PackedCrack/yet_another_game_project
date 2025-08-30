//
// Created by qwerty on 30/08/2025.
//
#pragma once

// Debug
#include <debug/Logger.hpp>
#include <debug/debug_defines.hpp>
// vulkan
#include <vulkan/vulkan.h>
//
//
namespace odin::graphics::vk::resource
{
template<typename dervied_t>
class DynamicBuffer
{
public:
    DynamicBuffer(VkDeviceSize partitionSize, std::uint64_t numPartitions)
        : m_PartitionSize{ partitionSize }
        , m_NumPartitions{ numPartitions } {};
public:
    [[nodiscard]] VkDeviceSize range() const { return m_PartitionSize; }
    [[nodiscard]] VkDeviceSize offset(std::uint64_t frameID) const
    {
        return static_cast<VkDeviceSize>(m_PartitionSize * (frameID % m_NumPartitions));
    }
    template<typename data_t>
    void write(std::span<const data_t> content, std::uint64_t frameID)
    {
        ODIN_ASSERT(content.size() * sizeof(data_t) < m_PartitionSize);
        write_to_buffer(content, offset(frameID));
    }
private:
    VkDeviceSize m_PartitionSize;
    std::uint64_t m_NumPartitions;
};
}    // namespace odin::graphics::vk::resource
