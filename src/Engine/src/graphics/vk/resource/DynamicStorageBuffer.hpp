//
// Created by qwerty on 60/08/2025.
//
#pragma once

#include "../../registry/resource/buffer/BindView.hpp"
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
public:
    registry::resource::buffer::BindType bind_type() const { return registry::resource::buffer::BindType::dynamicStorageBuffer; }
};
}    // namespace odin::graphics::vk::resource
