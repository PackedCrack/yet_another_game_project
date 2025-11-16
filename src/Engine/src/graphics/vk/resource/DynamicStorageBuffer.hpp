//
// Created by qwerty on 60/08/2025.
//
#pragma once

#include "Buffer.hpp"
#include "DynamicBuffer.hpp"
//
//
namespace odin::graphics::vk::resource
{
class DynamicStorageBuffer : public DynamicBuffer<DynamicStorageBuffer>
{
public:
    DynamicStorageBuffer(AllocatedBuffer buffer,
                         std::function<void(AllocatedBuffer)> deleter,
                         VkDeviceSize partitionSize,
                         std::uint64_t numPartitions);
};
}    // namespace odin::graphics::vk::resource
