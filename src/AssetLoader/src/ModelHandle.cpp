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
    : m_pSlot{ std::move(pSlot) }
{
    ODIN_ASSERT(m_pSlot != nullptr);
}
std::shared_ptr<const SceneGraph> ModelHandle::acquire() const
{
    return m_pSlot->pGraph.load();
}
}    // namespace asl
