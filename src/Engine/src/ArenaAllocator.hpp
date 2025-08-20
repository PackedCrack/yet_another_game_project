//
// Created by qwerty on 20/08/2025.
//

#pragma once

#include "BumpAllocator.hpp"
// std
#include <algorithm>
#include <utility>
#include <vector>
//
//
namespace odin
{
class ArenaAllocation
{
    using Deleter = std::function<void(std::uint64_t, std::uint64_t)>;
public:
    ArenaAllocation(std::uint64_t offset, std::uint64_t size, Deleter deleter)
        : m_Offset{ offset }
        , m_Size{ size }
        , m_Deleter{ std::move(deleter) }
    {}
public:
    ~ArenaAllocation()
    {
        if (m_Deleter)
        {
            m_Deleter(m_Offset, m_Size);
        }
    }
    ArenaAllocation(const ArenaAllocation& other) = delete;
    ArenaAllocation(ArenaAllocation&& other) noexcept
        : m_Offset{ other.m_Offset }
        , m_Size{ other.m_Size }
        , m_Deleter{ std::move(other.m_Deleter) }
    {}
    ArenaAllocation& operator=(const ArenaAllocation& other) = delete;
    ArenaAllocation& operator=(ArenaAllocation&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            m_Offset = other.m_Offset;
            m_Size = other.m_Size;
            m_Deleter = std::move(other.m_Deleter);
        }
        return *this;
    }
public:
    [[nodiscard]] std::uint64_t start() const
    {
        return m_Offset;
    }
    [[nodiscard]] std::uint64_t size() const
    {
        return m_Size;
    }
private:
    std::uint64_t m_Offset;
    std::uint64_t m_Size;
    Deleter m_Deleter;
};
//
//
template<typename element_t>
    requires std::is_trivially_copyable_v<element_t>
class ArenaAllocator
{
    using SubAllocator = BumpAllocator<element_t>;
    using BufferMirror = std::vector<element_t>;
    using Iterator = BufferMirror::iterator;
public:
    using ByteOffset = SubAllocator::ByteOffset;
    using ElementOffset = SubAllocator::ByteOffset;
    struct FreeBlock
    {
        ByteOffset start;
        std::uint64_t size;
        friend bool operator<(const FreeBlock& lhs, const FreeBlock& rhs)
        {
            return lhs.start < rhs.start;
        }
    };
public:
    // ArenaAllocator has to be heap allocated because it requries a stable This pointer.
    [[nodiscard]] static std::unique_ptr<ArenaAllocator<element_t>> make_arena_allocator(std::size_t elementCapacity, std::uint64_t minAlignment = 4)
    {
        using ArenaAllocator = ArenaAllocator<element_t>;

        return std::make_unique<ArenaAllocator>(ArenaAllocator{ elementCapacity, minAlignment });
    }
private:
    ArenaAllocator(std::size_t elementCapacity, std::uint64_t minAlignment)
        : m_SubAllocator(elementCapacity, minAlignment)
        , m_Freeblocks()
    {};
public:
    [[nodiscard]] ArenaAllocation make_allocation(std::uint64_t numElements, std::uint64_t requiredSize)
    {
        auto [offset, allocationSize] = m_SubAllocator.bump(numElements);
        ODIN_ASSERT(requiredSize <= allocationSize);

        if (requiredSize < allocationSize)
        {
            FreeBlock b{};
            b.start = offset + requiredSize;
            b.size = allocationSize - requiredSize;
            m_Freeblocks.emplace(b);
        }

        return ArenaAllocation{ offset, requiredSize, make_deleter() };
    }
    template<typename... ctor_arg_t>
    [[nodiscard]] ArenaAllocation insert(ctor_arg_t&&... args)
    {
        std::uint64_t requiredSize = sizeof(element_t);
        if (std::optional<FreeBlock> block = find_free_block(requiredSize); block.has_value())
        {
            return ArenaAllocation{ block->start, block->size, make_deleter() };
        }

        return make_allocation(1, requiredSize);
    }
    [[nodiscard]] ArenaAllocation insert_range(std::span<const element_t>& content)
    {
        ByteOffset requiredSize = content.size() * sizeof(element_t);
        if (std::optional<FreeBlock> block = find_free_block(requiredSize); block.has_value())
        {
            return ArenaAllocation{ block->start, block->size, make_deleter() };
        }

        return make_allocation(content.size(), requiredSize);
    }
    [[nodiscard]] ElementOffset element_offset(ByteOffset value)
    {
        return m_SubAllocator.element_offset(value);
    }
private:
    void insert_free_block(std::uint64_t start, std::uint64_t size)
    {
        auto [pBlock, emplaced] = m_Freeblocks.emplace(start, size);
        assert(emplaced);

        merge_free_blocks();
    }
    void merge_free_blocks()
    {
        using Iterator = typename std::set<FreeBlock>::iterator;

        if (m_Freeblocks.size() < 2)
        {
            return;
        }

        std::set<FreeBlock> updated{};

        Iterator pBlock = std::begin(m_Freeblocks);
        FreeBlock newBlock = *pBlock;
        while (pBlock != std::end(m_Freeblocks))
        {
            Iterator pNext = std::next(pBlock);
            if (pNext != std::end(m_Freeblocks))
            {
                if (pBlock->start + pBlock->size == pNext->start)
                {
                    newBlock.size += pNext->size;
                    pBlock = std::next(pBlock);
                }
                else
                {
                    updated.emplace(newBlock);
                    pBlock = std::next(pBlock);
                    newBlock = *pBlock;
                }
            }
            else
            {
                updated.emplace(newBlock);
                pBlock = std::end(m_Freeblocks);
            }
        }

        m_Freeblocks = std::move(updated);
    }
    [[nodiscard]] std::optional<FreeBlock> best_fit(std::uint64_t requiredSize)
    {
        std::optional<FreeBlock> bestBlock = std::nullopt;
        for (auto&& block : m_Freeblocks)
        {
            std::uint64_t realStart = m_SubAllocator.aligned_value(block.start);
            std::uint64_t end = block.start + block.size;
            if (realStart < end)
            {
                std::uint64_t realSize = end - realStart;
                if (requiredSize <= realSize)
                {
                    std::uint64_t lowestDifference = bestBlock != std::nullopt ? bestBlock->size - requiredSize : UINT64_MAX;
                    std::uint64_t foundDifference = realSize - requiredSize;
                    if (foundDifference < lowestDifference)
                    {
                        bestBlock = std::make_optional(block);
                    }
                }
            };
        }

        if (bestBlock)
        {
            m_Freeblocks.erase(*bestBlock);
        }

        return bestBlock;
    }
    [[nodiscard]] bool requires_realignment(const FreeBlock& block)
    {
        return block.start != m_SubAllocator.aligned_value(block.start);
    }
    [[nodiscard]] FreeBlock realign_block(FreeBlock block)
    {
        std::uint64_t alignedStart = m_SubAllocator.aligned_value(block.start);
        std::uint64_t cutSize = alignedStart - block.start;
        if (cutSize > 0)
        {
            m_Freeblocks.emplace(block.start, cutSize);
        }

        FreeBlock realignedBlock{};
        realignedBlock.start = alignedStart;
        realignedBlock.size = block.size - cutSize;

        return realignedBlock;
    }
    [[nodiscard]] bool has_excessive_tail(FreeBlock block, std::uint64_t requiredSize)
    {
        return requiredSize < block.size;
    }
    [[nodiscard]] FreeBlock cut_tail(FreeBlock block, std::uint64_t requiredSize)
    {
        assert(requiredSize < block.size);

        std::uint64_t tailStart = block.start + requiredSize;
        std::uint64_t tailSize = block.size - requiredSize;
        m_Freeblocks.emplace(tailStart, tailSize);

        block.size = requiredSize;
        return block;
    }
    [[nodiscard]] std::optional<FreeBlock> find_free_block(std::uint64_t requiredSize)
    {
        std::optional<FreeBlock> bestBlock = best_fit(requiredSize);
        if (!bestBlock)
        {
            return std::nullopt;
        }


        FreeBlock block = bestBlock.value();
        if (requires_realignment(block))
        {
            block = realign_block(block);
        }

        if (has_excessive_tail(block, requiredSize))
        {
            block = cut_tail(block, requiredSize);
        }

        return std::make_optional(block);
    }
    [[nodiscard]] std::function<void(std::uint64_t, std::uint64_t)> make_deleter()
    {
        return [this] (std::uint64_t offset, std::uint64_t size)
        {
            this->insert_free_block(offset, size);
        };
    }
private:
    SubAllocator m_SubAllocator;
    std::set<FreeBlock> m_Freeblocks;
};
}	// namespace odin