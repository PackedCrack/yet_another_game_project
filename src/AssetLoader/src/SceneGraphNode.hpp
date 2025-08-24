//
// Created by qwerty on 22/07/2024.
//

#pragma once

#include "common_types.hpp"
#include "Mesh.hpp"
// common
#include <CGraph.hpp>
//
//
namespace asl
{
static std::size_t TEMPORARY = 0;
class SceneGraphNode : public common::GraphVertex<SceneGraphNode>
{
public:
    SceneGraphNode(const TRS& transform, std::optional<Mesh> mesh);
    friend bool operator==(const SceneGraphNode& lhs, const SceneGraphNode& rhs);
    friend bool operator!=(const SceneGraphNode& lhs, const SceneGraphNode& rhs);
public:
    [[nodiscard]] const std::optional<Mesh>& geometry() const;
    [[nodiscard]] NodeView view() const;
private:
    std::size_t id{ TEMPORARY++ };
    TRS m_Transform;
    std::optional<Mesh> m_Geometry;
};
}    // namespace asl
