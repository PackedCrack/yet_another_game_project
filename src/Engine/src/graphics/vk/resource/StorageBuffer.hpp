//
// Created by qwerty on 12/08/2025.
//
#pragma once

#include "Buffer.hpp"
//
//
namespace odin::graphics::vk::resource
{
class StorageBuffer : public Buffer<StorageBuffer>
{
public:
    StorageBuffer(AllocatedBuffer buffer, std::function<void(AllocatedBuffer)> deleter);
};
}    // namespace odin::graphics::vk::resource
