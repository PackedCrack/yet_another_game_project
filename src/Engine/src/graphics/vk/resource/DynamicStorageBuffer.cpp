//
// Created by qwerty on 30/08/2025.
//
#include "DynamicStorageBuffer.hpp"
//
//
namespace odin::graphics::vk::resource
{
DynamicStorageBuffer::DynamicStorageBuffer(AllocatedBuffer buffer,
                                           std::function<void(AllocatedBuffer)> deleter,
                                           VkDeviceSize partitionSize,
                                           std::uint64_t numPartitions)
    : Buffer<DynamicStorageBuffer>{ buffer, std::move(deleter) }
    , DynamicBuffer<DynamicStorageBuffer>{ partitionSize, numPartitions } {};
}    // namespace odin::graphics::vk::resource
