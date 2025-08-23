//
// Created by qwerty on 22/08/2025.
//
#pragma once

#include "ECS.hpp"
//
//
namespace odin
{
[[nodiscard]] Entity ECS::make_entity()
{
    return { m_Registry.create(), std::addressof(m_Registry) };
}
}    // namespace odin
