//
// Created by qwerty on 22/07/2024.
//
#include "ModelNode.hpp"
//
//
namespace asl
{
ModelNode::ModelNode(const TRS& transform, std::optional<Mesh> mesh)
    : m_Transform{ transform }
    , m_Geometry{ std::move(mesh) }
{}
bool operator==(const ModelNode& lhs, const ModelNode& rhs)
{
    return lhs.id == rhs.id;
}
bool operator!=(const ModelNode& lhs, const ModelNode& rhs)
{
    return lhs.id != rhs.id;
}
const std::optional<Mesh>& ModelNode::geometry() const
{
    return m_Geometry;
}
NodeView ModelNode::view() const
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
