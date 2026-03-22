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
class DynamicUniformBuffer : public DynamicBuffer<DynamicUniformBuffer>
{
public:
    DynamicUniformBuffer(AllocatedBuffer buffer,
                         std::function<void(AllocatedBuffer)> deleter,
                         VkDeviceSize partitionSize,
                         std::uint64_t numPartitions);
public:
    registry::resource::buffer::BindType bind_type() const { return registry::resource::buffer::BindType::dynamicUniformBuffer; }
};
}    // namespace odin::graphics::vk::resource
