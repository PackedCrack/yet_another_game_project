//
// Created by qwerty on 22/08/2025.
//
#pragma once

#include "ECS.hpp"
//
//
namespace odin::details
{
[[nodiscard]] Entity ECS::make_entity()
{
    return m_Registry.create();
}
ECS& get_ecs()
{
    static ECS ecs{};
    return ecs;
}
}    // namespace odin::details
//
//
namespace odin
{
[[nodiscard]] Entity make_entity()
{
    details::ECS& ecs = details::get_ecs();
    return { ecs.make_entity() };
}
}    // namespace odin
