//
// Created by qwerty on 20/08/2025.
//
#pragma once

#include <debug/Logger.hpp>
// std
#include <cstdint>
#include <utility>
//
//
namespace odin
{
template<typename element_t>
class BumpAllocator
{
public:
    using ByteOffset = std::uint64_t;
    using ElementOffset = std::uint64_t;
public:
    BumpAllocator(std::uint64_t elementCapacity, std::uint64_t minAlignment)
        : m_Capacity{ elementCapacity * sizeof(element_t) }
        , m_InsertOffset{ 0u }
        , m_Alignment{ minAlignment }
    {
        if (m_Alignment < alignof(element_t))
        {
            m_Alignment = alignof(element_t);
        }
        m_Capacity = aligned_value(m_Capacity);
    };
    ~BumpAllocator() = default;
    BumpAllocator(const BumpAllocator& other) = delete;
    BumpAllocator(BumpAllocator&& other) noexcept
        : m_Capacity{ other.m_Capacity }
        , m_InsertOffset{ other.m_InsertOffset }
        , m_Alignment{ other.m_Alignment } {};
    BumpAllocator& operator=(const BumpAllocator& other) = delete;
    BumpAllocator& operator=(BumpAllocator&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            m_Capacity = other.m_Capacity;
            m_InsertOffset = other.m_InsertOffset;
            m_Alignment = other.m_Alignment;
        }

        return *this;
    };
    [[nodiscard]] std::tuple<ByteOffset, ByteOffset> bump(std::uint64_t numElements)
    {
        ODIN_ASSERT(numElements > 0);

        std::size_t bytesToAllocate = aligned_value(numElements * sizeof(element_t));
        std::uint64_t insertAt = aligned_value(m_InsertOffset);

        std::uint64_t bytesRemain = m_Capacity - insertAt;
        if (bytesToAllocate > bytesRemain)
        {
            LOG_FATAL("Buffer tracked by Bump Allocator ran out of memory!");
        }

        m_InsertOffset = insertAt + bytesToAllocate;

        return { insertAt, bytesToAllocate };
    }
    [[nodiscard]] ElementOffset element_offset(ByteOffset value) { return value / aligned_value(sizeof(element_t)); }
    [[nodiscard]] std::uint64_t aligned_value(std::uint64_t value)
    {
        // Black magic
        // https://stackoverflow.com/questions/45213511/formula-for-memory-alignment
        return (value + (m_Alignment - 1) & ~(m_Alignment - 1));
    }
private:
    std::uint64_t m_Capacity;
    std::uint64_t m_InsertOffset;
    std::uint64_t m_Alignment;
};
}    // namespace odin
