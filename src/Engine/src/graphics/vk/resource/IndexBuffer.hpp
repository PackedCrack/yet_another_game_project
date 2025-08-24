//
// Created by qwerty on 22/08/2025.
//
#pragma once

#include "Buffer.hpp"
#include "../CommandBuffer.hpp"
// glm
#include <glm/glm.hpp>
//
//
namespace odin::graphics::vk::resource
{
class IndexBuffer : public Buffer<IndexBuffer>
{
public:
    using index_t = std::uint16_t;
    IndexBuffer(AllocatedBuffer buffer, std::function<void(AllocatedBuffer)> deleter);
public:
    void bind(CommandBufferRef cmdBuffer) const;
    [[nodiscard]] std::size_t capacity() const;
};
}    // namespace odin::graphics::vk::resource
