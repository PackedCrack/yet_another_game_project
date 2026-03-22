//
// Created by qwerty on 07/09/2025.
//
#pragma once

#include "BindView.hpp"
#include "../../../vk/resource/Buffer.hpp"
#include "../../../vk/resource/StorageBuffer.hpp"

#include <concepts>
//
//
namespace odin::graphics::registry::resource::buffer
{
template<typename buffer_t>
concept VulkanBuffer = requires(const buffer_t& buffer) {
    { buffer.bind_type() } -> std::same_as<BindType>;
    { buffer.handle() } -> std::same_as<vk::resource::BufferRef>;
    { buffer.byte_capacity() } -> std::same_as<VkDeviceSize>;
};
template<typename buffer_t>
concept DynamicVulkanBuffer =
    VulkanBuffer<buffer_t> &&
    requires(const buffer_t& cbuffer, buffer_t& buffer, std::span<const std::uint8_t> data, std::uint64_t frameID) {
        { cbuffer.range() } -> std::same_as<VkDeviceSize>;
        { cbuffer.offset(frameID) } -> std::same_as<VkDeviceSize>;
        { buffer.write(data, frameID) } -> std::same_as<void>;
    };
template<typename buffer_t>
requires std::derived_from<buffer_t, vk::resource::Buffer<buffer_t>>
class BufferHandle
{
public:
    BufferHandle(std::shared_ptr<buffer_t> pBuffer)
        : m_pBuffer{ std::move(pBuffer) }
    {}
    buffer_t& operator*() { return *m_pBuffer; }
    buffer_t* operator->() { return m_pBuffer.get(); }
    const buffer_t& operator*() const { return *m_pBuffer; }
    const buffer_t* operator->() const { return m_pBuffer.get(); }
    [[nodiscard]] BindView to_view() const
    requires(VulkanBuffer<buffer_t> && !DynamicVulkanBuffer<buffer_t>)
    {
        BindView view{};
        view.type = m_pBuffer->bind_type();
        view.handle = m_pBuffer->handle().handle;
        view.offset = 0;
        view.range = m_pBuffer->byte_capacity();

        return view;
    }
    [[nodiscard]] BindView to_view(std::uint64_t frameID) const
    requires DynamicVulkanBuffer<buffer_t>
    {
        BindView view{};
        view.type = m_pBuffer->bind_type();
        view.handle = m_pBuffer->handle().handle;
        view.offset = m_pBuffer->offset(frameID);
        view.range = m_pBuffer->range();

        return view;
    }
private:
    std::shared_ptr<buffer_t> m_pBuffer;
};
}    // namespace odin::graphics::registry::resource::buffer
