//
// Created by qwerty on 12/08/2025.
//
#pragma once

#include "../../registry/resource/buffer/BindView.hpp"
#include "Buffer.hpp"
//
//
namespace odin::graphics::vk::resource
{
class StorageBuffer : public Buffer<StorageBuffer>
{
public:
    StorageBuffer(AllocatedBuffer buffer, std::function<void(AllocatedBuffer)> deleter);
public:
    registry::resource::buffer::BindType bind_type() const { return registry::resource::buffer::BindType::storageBuffer; }
};
}    // namespace odin::graphics::vk::resource
