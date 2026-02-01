#include "Core.hpp"

#include "ECS.hpp"
#include "component/DirtyFlag.hpp"
//
//
namespace odin
{
void translate(Entity e, const glm::vec3& delta)
{
    details::ECS& ecs = details::get_ecs();
    auto [local, flag] = ecs.get<component::LocalTRS, component::DirtyFlag>(e);
    local.translation += delta;
    flag.dirty = true;
}
void rotate(Entity e, float degree, glm::vec3 axis)
{
    glm::quat delta = glm::angleAxis(glm::radians(degree), glm::normalize(axis));
    rotate(e, delta);
}
void rotate(Entity e, const glm::quat& orientation)
{
    details::ECS& ecs = details::get_ecs();
    auto [local, flag] = ecs.get<component::LocalTRS, component::DirtyFlag>(e);
    local.orientation *= orientation;
    glm::normalize(local.orientation);
    flag.dirty = true;
}
void scale(Entity e, float scale)
{
    details::ECS& ecs = details::get_ecs();
    auto [local, flag] = ecs.get<component::LocalTRS, component::DirtyFlag>(e);
    local.scale *= scale;
    flag.dirty = true;
}
void set_local_trs(Entity e, const component::LocalTRS& trs)
{
    details::ECS& ecs = details::get_ecs();
    auto [local, flag] = ecs.get<component::LocalTRS, component::DirtyFlag>(e);
    local = trs;
    flag.dirty = true;
}
}    // namespace odin
