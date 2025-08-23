//
// Created by qwerty on 12/08/2025.
//
#include "UniformBuffer.hpp"
//
//
namespace odin::graphics::vk::resource
{
UniformBuffer::UniformBuffer(AllocatedBuffer buffer, std::function<void(AllocatedBuffer)> deleter)
    : Buffer<UniformBuffer>{ buffer, std::move(deleter) } {};
}    // namespace odin::graphics::vk::resource
