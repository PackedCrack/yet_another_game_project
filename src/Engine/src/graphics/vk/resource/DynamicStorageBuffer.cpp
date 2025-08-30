//
// Created by qwerty on 30/08/2025.
//
#include "DynamicStorageBuffer.hpp"
//
//
namespace odin::graphics::vk::resource
{
// clang-format off
DynamicStorageBuffer::DynamicStorageBuffer(AllocatedBuffer buffer,
                                           std::function<void(AllocatedBuffer)> deleter,
                                           VkDeviceSize partitionSize,
                                           std::uint64_t numPartitions)
    : DynamicBuffer<DynamicStorageBuffer>{ buffer, std::move(deleter), partitionSize, numPartitions }
{};
// clang-format on
}    // namespace odin::graphics::vk::resource
