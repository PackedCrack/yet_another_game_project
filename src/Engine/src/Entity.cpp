//
// Created by qwerty on 22/08/2025.
//
#include "Entity.hpp"
//
//
namespace odin
{
Entity::Entity(Entity::underlying_t id /*, underlying_registry_t* pRegistry*/)
    : m_ID{ id }
//, m_pRegistry{ pRegistry }
{}
Entity::underlying_t Entity::to_underlying() const
{
    return m_ID;
}
bool operator==(Entity lhs, Entity rhs) noexcept
{
    return lhs.to_underlying() == rhs.to_underlying();
}
bool operator!=(Entity lhs, Entity rhs) noexcept
{
    return lhs.to_underlying() != rhs.to_underlying();
}
}    // namespace odin
