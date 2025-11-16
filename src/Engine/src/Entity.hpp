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
public:
    Entity() = default;
    Entity(underlying_t id);
    friend bool operator==(Entity lhs, Entity rhs) noexcept;
    friend bool operator!=(Entity lhs, Entity rhs) noexcept;
public:
    [[nodiscard]] underlying_t to_underlying() const;
private:
    underlying_t m_ID = null_t{};
};
}    // namespace odin
