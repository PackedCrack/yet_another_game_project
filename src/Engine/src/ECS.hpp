//
// Created by qwerty on 22/08/2025.
//
#pragma once

#include "Entity.hpp"
//
//
namespace odin::details
{
//
//
class ECS
{
public:
    using underlying_registry_t = entt::registry;
public:
    template<typename... component_t, typename buffer_t, typename invocable_t>
    requires std::ranges::contiguous_range<buffer_t> && std::invocable<invocable_t, Entity, component_t&...>
    void for_each(buffer_t&& entities, invocable_t&& action)
    {
        static_assert(std::same_as<typename std::remove_cvref_t<buffer_t>::value_type, Entity>);

        auto view = m_Registry.template view<component_t...>();
        auto&& func = std::forward<invocable_t>(action);
        for (auto&& entity : entities)
        {
            Entity e = entity;
            std::invoke(func, e, view.template get<component_t>(e.to_underlying())...);
        }
    }
    template<typename... component_t, typename invocable_t>
    requires std::invocable<invocable_t, Entity, component_t&...>
    void for_each(invocable_t&& action)
    {
        auto view = m_Registry.template view<component_t...>();
        auto&& func = std::forward<invocable_t>(action);
        for (auto e : view)
        {
            Entity entity{ e };
            std::invoke(func, entity, view.template get<component_t>(entity.to_underlying())...);
        }
    }
    template<typename state_t>
    void emplace_global_state()
    {
        m_Registry.ctx().emplace<state_t>();
    }
    template<typename state_t>
    [[nodiscard]] state_t& global_state()
    {
        return m_Registry.ctx().get<state_t>();
    }
    [[nodiscard]] Entity make_entity();
    template<typename... component_t>
    [[nodiscard]] decltype(auto) get(Entity e)
    {
        return m_Registry.template get<component_t...>(e.to_underlying());
    }
    template<typename component_t, typename... component_arg_t>
    component_t& emplace(Entity entity, component_arg_t&&... args)
    {
        return m_Registry.emplace<component_t>(entity.to_underlying(), std::forward<component_arg_t>(args)...);
    }
private:
    underlying_registry_t m_Registry;
};
[[nodiscard]] ECS& get_ecs();
}    // namespace odin::details
//
//
namespace odin
{
template<typename component_t, typename... component_arg_t>
component_t& emplace_component(Entity entity, component_arg_t&&... args)
{
    details::ECS& ecs = details::get_ecs();
    return ecs.emplace<component_t>(entity, std::forward<component_arg_t>(args)...);
}
template<typename... component_t>
[[nodiscard]] decltype(auto) get_components(Entity e)
{
    details::ECS& ecs = details::get_ecs();
    return ecs.get<component_t...>(e);
}
template<typename... component_t, typename buffer_t, typename invocable_t>
requires std::ranges::contiguous_range<buffer_t> && std::invocable<invocable_t, Entity, component_t&...>
void for_each_component(buffer_t&& entities, invocable_t&& action)
{
    details::ECS& ecs = details::get_ecs();
    ecs.for_each<component_t...>(std::forward<buffer_t>(entities), std::forward<invocable_t>(action));
}
template<typename... component_t, typename invocable_t>
requires std::invocable<invocable_t, Entity, component_t&...>
void for_each_component(invocable_t&& action)
{
    details::ECS& ecs = details::get_ecs();
    ecs.for_each<component_t...>(std::forward<invocable_t>(action));
}
[[nodiscard]] Entity make_entity();
}    // namespace odin
