//
// Created by qwerty on 07/09/2025.
//
#pragma once

#include "BindView.hpp"
#include "../../../vk/resource/Buffer.hpp"
#include "../../../vk/resource/StorageBuffer.hpp"
//
//
namespace odin::graphics::registry::resource::buffer
{
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
    {
        BindView view{};
        if constexpr (std::same_as<buffer_t, vk::resource::StorageBuffer>)
        {
            view.type = BindType::storage;
        }
        else
        {
            static_assert(false);
        }

        view.handle = m_pBuffer->handle().handle;
        view.offset = 0;
        view.range = m_pBuffer->byte_capacity();
        return view;
    }
private:
    std::shared_ptr<buffer_t> m_pBuffer;
};
}    // namespace odin::graphics::registry::resource::buffer
