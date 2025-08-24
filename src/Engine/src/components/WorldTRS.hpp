//
// Created by qwerty on 22/08/2025.
//
#pragma once

// common
#include <glm_headers.hpp>
//
//
namespace odin::component
{
struct WorldTRS
{
    glm::quat orientation;
    glm::vec3 translation;
    float scale;
};
}    // namespace odin::component
