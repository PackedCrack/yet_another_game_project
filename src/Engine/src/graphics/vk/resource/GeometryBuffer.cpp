//
// Created by qwerty on 19/08/2025.
//
#include "GeometryBuffer.hpp"
namespace odin::graphics::vk::resource
{
GeometryBuffer::GeometryBuffer(std::uint64_t size)
    : m_BufferSize{ size }
    , m_TransferOffset{ 0u } {};
GeometryBuffer::GeometryBuffer(GeometryBuffer&& other) noexcept
    : m_BufferSize{ other.m_BufferSize }
    , m_TransferOffset{ other.m_TransferOffset } {};
GeometryBuffer& GeometryBuffer::operator=(GeometryBuffer&& other) noexcept
{
    if (this != std::addressof(other))
    {
        m_BufferSize = other.m_BufferSize;
        m_TransferOffset = other.m_TransferOffset;
    }

    return *this;
};
}    // namespace odin::graphics::vk::resource
