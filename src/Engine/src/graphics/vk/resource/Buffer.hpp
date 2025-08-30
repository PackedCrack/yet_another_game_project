//
// Created by qwerty on 18/08/2025.
//
#pragma once

// Debug
#include <debug/Logger.hpp>
#include <debug/debug_defines.hpp>
// vulkan
#include <vulkan/vulkan.h>
// std
#include <span>
//
//
namespace odin::graphics::vk::resource
{
struct AllocatedBuffer
{
    VkBuffer handle = VK_NULL_HANDLE;
    void* pAllocation = nullptr;
    void* pData = nullptr;
    VkDeviceSize size;
    VkDeviceSize minAlignment;
};
struct BufferRef
{
    VkBuffer handle;
};
template<typename dervied_t>
class Buffer
{
public:
    Buffer(AllocatedBuffer buffer, std::function<void(AllocatedBuffer)> deleter)
        : m_Buffer{ buffer }
        , m_Deleter{ std::move(deleter) }
    {
        ODIN_ASSERT(m_Buffer.handle != VK_NULL_HANDLE);
        ODIN_ASSERT(m_Buffer.pAllocation != nullptr);
        ODIN_ASSERT(m_Deleter);
    };
    ~Buffer()
    {
        if (m_Buffer.handle)
        {
            m_Deleter(m_Buffer);
        }
    };
    Buffer(const Buffer& other) = delete;
    Buffer(Buffer&& other) noexcept
        : m_Buffer{}
        , m_Deleter{}
    {
        std::swap(m_Buffer, other.m_Buffer);
        std::swap(m_Deleter, other.m_Deleter);
    }
    Buffer& operator=(const Buffer& other) = delete;
    Buffer& operator=(Buffer&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            std::swap(m_Buffer, other.m_Buffer);
            std::swap(m_Deleter, other.m_Deleter);
        }

        return *this;
    }
public:
    [[nodiscard]] BufferRef handle() const { return BufferRef{ .handle = m_Buffer.handle }; };
    [[nodiscard]] VkDeviceSize byte_capacity() const { return m_Buffer.size; }
    [[nodiscard]] VkDeviceSize min_alignment() const { return m_Buffer.minAlignment; }
protected:
    template<typename data_t>
    void write_to_buffer(std::span<const data_t> content)
    {
        write_to_buffer_with_offset(content, 0);
    }
    template<typename data_t>
    void write_to_buffer_with_offset(std::span<const data_t> content, std::size_t offset)
    {
        ODIN_ASSERT(m_Buffer.pData != nullptr);
        std::byte* pData = static_cast<std::byte*>(m_Buffer.pData) + offset;
        std::memcpy(pData, content.data(), content.size() * sizeof(std::remove_cvref_t<data_t>));
    }
private:
    AllocatedBuffer m_Buffer;
    std::function<void(AllocatedBuffer)> m_Deleter;
};
}    // namespace odin::graphics::vk::resource
