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
class ModelNode : public common::GraphVertex<ModelNode>
{
public:
    //ModelNode() = default;
    ModelNode(const TRS& transform, std::optional<Mesh> mesh);
    friend bool operator==(const ModelNode& lhs, const ModelNode& rhs);
    friend bool operator!=(const ModelNode& lhs, const ModelNode& rhs);
public:
    [[nodiscard]] const std::optional<Mesh>& geometry() const;
    [[nodiscard]] NodeView view() const;
private:
    std::size_t id{ TEMPORARY++ };
    TRS m_Transform;
    std::optional<Mesh> m_Geometry;
};
}    // namespace asl
