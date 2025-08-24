//
// Created by qwerty on 22/08/2025.
//
#include "IndexBuffer.hpp"
//
//
namespace odin::graphics::vk::resource
{
IndexBuffer::IndexBuffer(AllocatedBuffer buffer, std::function<void(AllocatedBuffer)> deleter)
    : Buffer<IndexBuffer>{ buffer, std::move(deleter) }
{}
void IndexBuffer::bind(CommandBufferRef cmdBuffer) const
{
    BufferRef buffer = handle();
    vkCmdBindIndexBuffer(cmdBuffer.handle, buffer.handle, 0, VK_INDEX_TYPE_UINT16);
}
std::size_t IndexBuffer::capacity() const
{
    return byte_capacity() / sizeof(index_t);
}
}    // namespace odin::graphics::vk::resource
