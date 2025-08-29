//
// Created by qwerty on 26/07/2025.
//
#include "ShaderHandle.hpp"
//
//
namespace odin::graphics::registry::resource::shader
{
ShaderHandle::ShaderHandle(std::shared_ptr<ShaderSlot> pSlot, std::function<void()> hot_reload)
    : m_pSlot{ pSlot == nullptr ? nullptr : std::move(pSlot) }
    , PFN_hot_reload(std::move(hot_reload))
{}
std::shared_ptr<const vk::resource::ShaderModule> ShaderHandle::acquire() const
{ 
    if (PFN_hot_reload)
    {
        PFN_hot_reload();
    }
    return std::atomic_load(std::addressof(m_pSlot->pResource)); 
}
std::uintptr_t ShaderHandle::id() const
{
    return reinterpret_cast<std::uintptr_t>(m_pSlot.get());
}
}    // namespace odin::graphics::registry::resource::shader
