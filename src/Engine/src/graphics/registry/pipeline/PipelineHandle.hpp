//
// Created by qwerty on 30/08/2025.
//
#pragma once
//
//
namespace odin::graphics::registry::pipeline
{
template<typename derived_t, typename slot_t, typename resource_t>
struct PipelineHandle
{
    PipelineHandle(std::shared_ptr<slot_t> pSlot, std::function<void()> hot_reload)
        : m_pSlot{ pSlot == nullptr ? nullptr : std::move(pSlot) }
        , cb_hot_reload{ std::move(hot_reload) }
    {}
    std::shared_ptr<const resource_t> acquire() const 
    { 
        cb_hot_reload();
        return std::atomic_load(std::addressof(m_pSlot->pResource)); 
    }
    operator bool() const { return m_pSlot != nullptr; }
protected:
    std::shared_ptr<slot_t> m_pSlot;
    std::function<void()> cb_hot_reload;
};
}    // namespace odin::graphics::registry::pipeline
