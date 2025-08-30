//
// Created by qwerty on 26/08/2025.
//
#pragma once
//
//
namespace odin::graphics::registry
{
template<typename derived_t, typename slot_t, typename resource_t>
class Handle
{
public:
    Handle(std::shared_ptr<slot_t> pSlot)
        : m_pSlot{ pSlot == nullptr ? nullptr : std::move(pSlot) }
    {}
    std::shared_ptr<const resource_t> acquire() const { return std::atomic_load(std::addressof(m_pSlot->pResource)); }
    operator bool() const { return m_pSlot != nullptr; }
protected:
    std::shared_ptr<slot_t> m_pSlot;
};
}    // namespace odin::graphics::registry
