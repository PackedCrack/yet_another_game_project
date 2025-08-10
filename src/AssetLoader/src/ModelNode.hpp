//
// Created by qwerty on 22/07/2024.
//

#pragma once

#include "CGraph.hpp"
#include "Mesh.hpp"
//
//
namespace asl
{
struct TRS
{
    glm::quat rotation;
    glm::vec4 translation;
    glm::vec4 scale;
};
static std::size_t TEMPORARY = 0;
class ModelNode : public common::GraphVertex<ModelNode>
{
    std::size_t id{ TEMPORARY++ };
    TRS m_Transform;
    std::optional<Mesh> m_Geometry;
public:
    ModelNode() = default;
    ModelNode(const TRS& transform, std::optional<Mesh> mesh);
    friend bool operator==(const ModelNode& lhs, const ModelNode& rhs);
    friend bool operator!=(const ModelNode& lhs, const ModelNode& rhs);
};
}    // namespace asl
