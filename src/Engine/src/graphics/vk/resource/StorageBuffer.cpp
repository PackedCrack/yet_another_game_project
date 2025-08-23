//
// Created by qwerty on 12/08/2025.
//
#include "StorageBuffer.hpp"
//
//
namespace odin::graphics::vk::resource
{
StorageBuffer::StorageBuffer(AllocatedBuffer buffer, std::function<void(AllocatedBuffer)> deleter)
    : Buffer<StorageBuffer>{ buffer, std::move(deleter) } {};
}    // namespace odin::graphics::vk::resource
