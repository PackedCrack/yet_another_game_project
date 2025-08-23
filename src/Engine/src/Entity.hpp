//
// Created by qwerty on 22/08/2025.
//
#pragma once

//
//
#include <entt/entt.hpp>
//
//
namespace odin
{
class Entity
{
public:
    using underlying_t = entt::entity;
    using null_t = entt::null_t;
    using underlying_registry_t = entt::registry;
public:
    Entity(underlying_t id, underlying_registry_t* pRegistry);
    friend bool operator==(Entity lhs, Entity rhs) noexcept;
    friend bool operator!=(Entity lhs, Entity rhs) noexcept;
public:
    template<typename component_t, typename... component_arg_t>
    Entity& emplace(component_arg_t&&... args)
    {
        m_pRegistry->emplace<component_t>(m_ID, std::forward<component_arg_t>(args)...);
        return *this;
    }
    [[nodiscard]] underlying_t to_underlying() const;
private:
    underlying_t m_ID;
    underlying_registry_t* m_pRegistry = nullptr;
};
}    // namespace odin
