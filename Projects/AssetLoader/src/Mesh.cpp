//
// Created by qwerty on 22/07/2024.
//
#include "Mesh.h"
namespace asl
{
bool operator==(const Mesh& lhs, const Mesh& rhs)
{
    return lhs.id == rhs.id;
}
bool operator!=(const Mesh& lhs, const Mesh& rhs)
{
    return lhs.id != rhs.id;
}
Mesh::Mesh()
    : id{ counter++ }
    , numVertices{}
    , position{}
    , normal{}
    , color{}
    , uv{}
    , indices{}
    , metallic{ std::nullopt }
    , roughness{ std::nullopt }
    , baseTexture{ std::nullopt }
    , metallicRoughnessTexture{ std::nullopt }
    , normalTexture{ std::nullopt }
    , occlusionTexture{ std::nullopt }
    , emissiveTexture{ std::nullopt }
{}
}    // namespace asl
