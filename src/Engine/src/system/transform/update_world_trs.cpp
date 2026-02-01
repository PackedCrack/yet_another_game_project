//
// Created by qwerty on 01/02/2026.
//
#include "update_world_trs.hpp"

#include "../../component/Children.hpp"
#include "../../component/DirtyFlag.hpp"
#include "../../component/LocalTRS.hpp"
#include "../../component/Parent.hpp"
#include "../../component/WorldTRS.hpp"
#include "../../ECS.hpp"
// glm


namespace odin::system::transform
{
inline static void compose(const component::WorldTRS& pw, const component::LocalTRS& loc, component::WorldTRS& out)
{
    out.orientation = glm::normalize(pw.orientation * loc.orientation);
    out.scale = pw.scale * loc.scale;
    //out.translation = pw.translation + glm::rotate(pw.orientation, pw.scale * loc.translation);
    out.translation = pw.translation + (pw.orientation * (pw.scale * loc.translation));
}
// Recompute subtree starting at 'top'.
// 'parent_changed' must be true for 'top' if its parent's world changed, or if 'top' itself was dirty.
static void recompute_subtree(details::ECS& ecs, Entity top, bool parent_changed)
{
    // Non-recursive DFS
    struct Node
    {
        Entity e;
        bool parent_changed;
    };
    std::vector<Node> stack;
    stack.push_back({ top, parent_changed });

    // Track visited to avoid double work if subtrees overlap in the input set
    std::unordered_set<entt::entity> visited;

    while (!stack.empty())
    {
        Node n = stack.back();
        stack.pop_back();

        entt::entity raw = n.e.to_underlying();
        if (!visited.insert(raw).second)
        {
            continue;
        }

        auto& par = ecs.get<component::Parent>(n.e);
        auto& local = ecs.get<component::LocalTRS>(n.e);
        auto& world = ecs.get<component::WorldTRS>(n.e);
        auto& dirty = ecs.get<component::DirtyFlag>(n.e);

        const bool was_dirty = dirty.dirty;
        const bool must_update = n.parent_changed || was_dirty;

        if (!par.parent.has_value())
        {
            // root: world = local if needed
            if (must_update)
            {
                world.orientation = local.orientation;
                world.translation = local.translation;
                world.scale = local.scale;
            }
        }
        else
        {
            const auto& pw = ecs.get<component::WorldTRS>(*par.parent);
            if (must_update)
            {
                compose(pw, local, world);
            }
        }

        // Clear this node's dirty bit after using it
        dirty.dirty = false;

        // Children inherit "parent_changed" if this node updated
        const bool child_parent_changed = must_update;

        // Push children
        const auto& kids = ecs.get<component::Children>(n.e);
        for (Entity c : kids.entities)
        {
            // We only need to visit a child if either:
            // - its parent changed, or
            // - the child is itself dirty.
            const bool child_dirty = ecs.get<component::DirtyFlag>(c).dirty;
            if (child_parent_changed || child_dirty)
            {
                stack.push_back({ c, child_parent_changed });
            }
        }
    }
}
// WorldTRS update pass. Call once per frame before collecting draw data.
void update_world_trs()
{
    details::ECS& ecs = details::get_ecs();

    // 1) Collect dirty entities
    std::vector<Entity> dirty;
    dirty.reserve(256);
    ecs.for_each<component::DirtyFlag>(
        [&](Entity e, const component::DirtyFlag& f)
        {
            if (f.dirty)
            {
                dirty.push_back(e);
            }
        });

    if (dirty.empty())
    {
        return;
    }

    // 2) For each dirty entity, climb to the highest dirty ancestor whose parent is clean or null
    auto parent_of = [&](Entity e) -> std::optional<Entity> { return ecs.get<component::Parent>(e).parent; };
    auto is_dirty = [&](Entity e) -> bool { return ecs.get<component::DirtyFlag>(e).dirty; };

    std::vector<Entity> tops;
    tops.reserve(dirty.size());

    for (Entity e : dirty)
    {
        Entity top = e;
        while (true)
        {
            auto p = parent_of(top);
            if (!p.has_value())
            {
                break;    // reached root
            }
            if (!is_dirty(*p))
            {
                break;    // parent clean -> stop here
            }
            top = *p;    // climb further up
        }
        tops.push_back(top);
    }

    // Deduplicate tops
    std::sort(tops.begin(), tops.end(), [](Entity a, Entity b) { return a.to_underlying() < b.to_underlying(); });
    tops.erase(std::unique(tops.begin(), tops.end(), [](Entity a, Entity b) { return a.to_underlying() == b.to_underlying(); }),
               tops.end());

    // 3) Recompute each dirty subtree starting from its top
    for (Entity top : tops)
    {
        // If top has a clean parent, that parent's world is already valid.
        // Force update at 'top' because either its parent changed earlier in this pass or top itself is dirty.
        recompute_subtree(ecs, top, /*parent_changed=*/true);
    }
}
}    // namespace odin::system::transform
