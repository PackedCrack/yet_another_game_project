//
// Created by qwerty on 18/08/2025.
//
#pragma once

#include "Buffer.hpp"
//
//
namespace odin::graphics::vk::resource
{
class StagingBuffer : public Buffer<StagingBuffer>
{
public:
    StagingBuffer(AllocatedBuffer buffer, std::function<void(AllocatedBuffer)> deleter);
public:
    template<typename data_t>
    void write(std::span<data_t> content)
    {
        write_to_buffer<data_t>(content);
    }
};
}    // namespace odin::graphics::vk::resource
