//
// Created by qwerty on 22/08/2025.
//
#pragma once

#include "../ECS.hpp"
//
//
namespace odin::components
{
struct Parent
{
    std::optional<Entity> parent;
};
}    // namespace odin::components
