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
#include "glm_headers.hpp"
//
//
namespace
{
struct Node
{
    odin::Entity entity;
    bool parentChanged;
};
void compose(const odin::component::WorldTRS& pw, const odin::component::LocalTRS& loc, odin::component::WorldTRS& out)
{
    out.orientation = glm::normalize(pw.orientation * loc.orientation);
    out.scale = pw.scale * loc.scale;
    //out.translation = pw.translation + glm::rotate(pw.orientation, pw.scale * loc.translation);
    out.translation = pw.translation + (pw.orientation * (pw.scale * loc.translation));
}
[[nodiscard]] odin::component::WorldTRS& updated_world_trs(odin::Entity entity)
{
    const odin::component::LocalTRS& local = odin::get_components<odin::component::LocalTRS>(entity);
    odin::component::WorldTRS& world = odin::get_components<odin::component::WorldTRS>(entity);

    world.orientation = local.orientation;
    world.translation = local.translation;
    world.scale = local.scale;

    return world;
}
[[nodiscard]] bool requires_update(const Node& n)
{
    bool dirty = odin::get_components<odin::component::DirtyFlag>(n.entity).dirty;
    return n.parentChanged || dirty;
}
[[nodiscard]] Node pop_top(std::vector<Node>& stack)
{
    Node n = stack.back();
    stack.pop_back();

    return n;
}
[[nodiscard]] bool do_update(const Node& n)
{
    odin::Entity entity = n.entity;

    odin::component::WorldTRS& world = odin::get_components<odin::component::WorldTRS>(entity);

    bool shouldUpdate = requires_update(n);
    std::optional<odin::Entity> parent = odin::get_components<odin::component::Parent>(entity).parent;
    if (!parent)
    {
        // root: world = local if needed
        if (shouldUpdate)
        {
            world = updated_world_trs(entity);
        }
    }
    else
    {
        const odin::component::WorldTRS& parentWorld = odin::get_components<odin::component::WorldTRS>(*parent);
        if (shouldUpdate)
        {
            const odin::component::LocalTRS& local = odin::get_components<odin::component::LocalTRS>(entity);
            compose(parentWorld, local, world);
        }
    }

    // Clear this node's dirty bit after using it
    odin::component::DirtyFlag& dirty = odin::get_components<odin::component::DirtyFlag>(n.entity);
    dirty.dirty = false;

    return shouldUpdate;
}
void push_children(std::vector<Node>& outStack, odin::Entity parent, bool parentChanged)
{
    const odin::component::Children& children = odin::get_components<odin::component::Children>(parent);
    for (odin::Entity child : children.entities)
    {
        // Only need to visit a child if its parent changed, or the child is itself dirty.
        const bool childDirty = odin::get_components<odin::component::DirtyFlag>(child).dirty;
        if (parentChanged || childDirty)
        {
            outStack.push_back({ child, parentChanged });
        }
    }
}
void recompute_subtree(odin::Entity root)
{
    // Track visited to avoid double work if subtrees overlap in the input set
    using entity_t = odin::Entity::underlying_t;
    std::unordered_set<entity_t> visited{};
    std::vector<Node> stack{};
    stack.push_back({ root, true });

    while (!stack.empty())
    {
        Node n = pop_top(stack);

        odin::Entity entity = n.entity;
        if (!visited.insert(entity.to_underlying()).second)
        {
            continue;
        }

        // Children inherit "parentChanged" (in Node) if this node updated
        bool parentChanged = do_update(n);
        push_children(stack, entity, parentChanged);
    }
}
[[nodiscard]] std::optional<odin::Entity> parent(odin::Entity child)
{
    return odin::get_components<odin::component::Parent>(child).parent;
}
[[nodiscard]] bool is_root_or_clean(std::optional<odin::Entity> e)
{
    // No more parents - reached root
    if (!e.has_value())
    {
        return true;
    }
    bool isDirty = odin::get_components<odin::component::DirtyFlag>(*e).dirty;
    // Parent is clean - this entity is the dirty root
    if (!isDirty)
    {
        return true;
    }

    return false;
}
void remove_duplicates(std::vector<odin::Entity>& outDirtyRoots)
{
    std::ranges::sort(outDirtyRoots, {}, [](odin::Entity e) { return e.to_underlying(); });

    std::ranges::subrange sr = std::ranges::unique(outDirtyRoots, {}, [](odin::Entity e) { return e.to_underlying(); });

    outDirtyRoots.erase(std::begin(sr), std::end(sr));
}
[[nodiscard]] std::vector<odin::Entity> dirty_roots(std::span<const odin::Entity> dirtyEntities)
{
    std::vector<odin::Entity> dirtyRoots{};
    dirtyRoots.reserve(dirtyEntities.size());
    for (odin::Entity e : dirtyEntities)
    {
        odin::Entity top = e;
        while (true)
        {
            auto p = parent(top);
            if (is_root_or_clean(p))
            {
                break;
            }
            top = *p;    // climb further up
        }
        dirtyRoots.push_back(top);
    }

    remove_duplicates(dirtyRoots);
    return dirtyRoots;
}
[[nodiscard]] std::vector<odin::Entity> dirty_entities()
{
    std::vector<odin::Entity> dirty{};
    dirty.reserve(256);
    auto store_if_dirty = [&dirty](odin::Entity e, odin::component::DirtyFlag f)
    {
        if (f.dirty)
        {
            dirty.push_back(e);
        }
    };

    odin::for_each_component<odin::component::DirtyFlag>(store_if_dirty);

    return dirty;
}
}    // namespace
namespace odin::system::transform
{
void update_world_trs()
{
    // Collect dirty entities
    std::vector<Entity> dirty = dirty_entities();
    if (dirty.empty())
    {
        return;
    }
    // Store only the entities that are roots of dirty sub graphs
    std::vector<Entity> dirtyRoots = dirty_roots(dirty);

    for (Entity root : dirtyRoots)
    {
        // If root has a clean parent, that parent's world is already valid.
        // Force update at 'root' because either its parent changed earlier in this pass or top itself is dirty.
        recompute_subtree(root);
    }
}
}    // namespace odin::system::transform
