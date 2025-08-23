//
// Created by qwerty on 18/08/2025.
//
#include "StagingBuffer.hpp"
//
//
namespace
{}    // namespace
namespace odin::graphics::vk::resource
{
StagingBuffer::StagingBuffer(AllocatedBuffer buffer, std::function<void(AllocatedBuffer)> deleter)
    : Buffer<StagingBuffer>{ buffer, std::move(deleter) } {};
}    // namespace odin::graphics::vk::resource
