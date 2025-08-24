//
// Created by qwerty on 22/07/2024.
//
#include "SceneGraphNode.hpp"
//
//
namespace asl
{
SceneGraphNode::SceneGraphNode(const TRS& transform, std::optional<Mesh> mesh)
    : m_Transform{ transform }
    , m_Geometry{ std::move(mesh) }
{}
bool operator==(const SceneGraphNode& lhs, const SceneGraphNode& rhs)
{
    return lhs.id == rhs.id;
}
bool operator!=(const SceneGraphNode& lhs, const SceneGraphNode& rhs)
{
    return lhs.id != rhs.id;
}
const std::optional<Mesh>& SceneGraphNode::geometry() const
{
    return m_Geometry;
}
NodeView SceneGraphNode::view() const
{
    NodeView view{};
    view.local = m_Transform;
    view.geometry = std::nullopt;
    if (m_Geometry)
    {
        view.geometry = m_Geometry->view();
    }

    return view;
}
}    // namespace asl
