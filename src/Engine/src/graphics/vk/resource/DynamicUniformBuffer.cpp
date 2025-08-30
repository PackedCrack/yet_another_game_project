//
// Created by qwerty on 30/08/2025.
//
#include "DynamicUniformBuffer.hpp"
//
//
namespace odin::graphics::vk::resource
{
DynamicUniformBuffer::DynamicUniformBuffer(AllocatedBuffer buffer,
                                           std::function<void(AllocatedBuffer)> deleter,
                                           VkDeviceSize partitionSize,
                                           std::uint64_t numPartitions)
    : DynamicBuffer<DynamicUniformBuffer>{ buffer, std::move(deleter), partitionSize, numPartitions } {};
}    // namespace odin::graphics::vk::resource
