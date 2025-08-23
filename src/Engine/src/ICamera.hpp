//
// Created by qwerty on 17/08/2025.
//
#pragma once

#include <glm/glm.hpp>
//
//
namespace odin
{
struct ViewProjection
{
    glm::mat4 projection;
    glm::mat4 view;
};
class ICamera
{
    virtual ~ICamera() = 0;
    virtual ViewProjection view_projection() const = 0;
};
}    // namespace odin
