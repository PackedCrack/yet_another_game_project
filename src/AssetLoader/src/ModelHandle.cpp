//
// Created by qwerty on 24/07/2025.
//
#include "ModelHandle.hpp"

#include <debug/debug_defines.hpp>
//
//
namespace asl
{
ModelHandle::ModelHandle(std::shared_ptr<ModelSlot> pSlot)
    : m_pSlot{ pSlot == nullptr ? nullptr : std::move(pSlot) }
{}
std::shared_ptr<const SceneGraph> ModelHandle::acquire() const
{
    return std::atomic_load(std::addressof(m_pSlot->pGraph));
}
}    // namespace asl
