//
// Created by qwerty on 07/09/2025.
//
#pragma once

#include "BindView.hpp"
#include "../../../vk/resource/DynamicBuffer.hpp"
#include "../../../vk/resource/DynamicStorageBuffer.hpp"
#include "../../../vk/resource/DynamicUniformBuffer.hpp"
//
//
namespace odin::graphics::registry::resource::buffer
{
template<typename buffer_t>
requires std::derived_from<buffer_t, vk::resource::DynamicBuffer<buffer_t>>
class DynamicBufferHandle
{
public:
    DynamicBufferHandle(std::shared_ptr<buffer_t> pBuffer)
        : m_pBuffer{ std::move(pBuffer) }
    {}
    buffer_t& operator*() { return *m_pBuffer; }
    buffer_t* operator->() { return m_pBuffer.get(); }
    const buffer_t& operator*() const { return *m_pBuffer; }
    const buffer_t* operator->() const { return m_pBuffer.get(); }
    [[nodiscard]] BindView to_view(std::uint64_t frameID) const
    {
        BindView view{};
        if constexpr (std::same_as<buffer_t, vk::resource::DynamicStorageBuffer>)
        {
            view.type = BindType::dynamicStorage;
        }
        else if constexpr (std::same_as<buffer_t, vk::resource::DynamicUniformBuffer>)
        {
            view.type = BindType::dynamicUniform;
        }
        else
        {
            static_assert(false);
        }

        view.handle = m_pBuffer->handle().handle;
        view.offset = m_pBuffer->offset(frameID);
        view.range = m_pBuffer->range();
        return view;
    }
private:
    std::shared_ptr<buffer_t> m_pBuffer;
};
}    // namespace odin::graphics::registry::resource::buffer
