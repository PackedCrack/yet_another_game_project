//
// Created by qwerty on 07/11/2025.
//
#pragma once

#include "Entity.hpp"
#include "component/LocalTRS.hpp"
//
//
namespace odin
{
void translate(Entity e, const glm::vec3& delta);
void rotate(Entity e, float degree, glm::vec3 axis);
void rotate(Entity e, const glm::quat& orientation);
void scale(Entity e, float scale);
void set_local_trs(Entity e, const component::LocalTRS& trs);
}    // namespace odin
