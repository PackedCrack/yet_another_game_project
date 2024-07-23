//
// Created by qwerty on 22/07/2024.
//

#pragma once

#include "CGraph.hpp"
#include "glm_headers.h"
//
//
namespace asl
{
struct TextureData
{
    std::vector<uint8_t> imageBuffer;
    int32_t imageWidth;
    int32_t imageHeight;
};
struct Mesh : public common::GraphVertex<Mesh>
{
    Mesh();
    std::size_t counter;    // replace this with UUID
    std::size_t id;

    uint32_t numVertices;
    std::vector<glm::vec3> position;
    std::vector<glm::vec3> normal;
    std::vector<glm::vec4> color;
    std::vector<glm::vec2> uv;
    std::vector<uint16_t> indices;
    std::optional<glm::mat4> matrix;

    std::optional<double> metallic;
    std::optional<double> roughness;
    std::optional<TextureData> baseTexture;
    std::optional<TextureData> metallicRoughnessTexture;
    std::optional<TextureData> normalTexture;
    std::optional<TextureData> occlusionTexture;
    std::optional<TextureData> emissiveTexture;
    friend bool operator==(const Mesh& lhs, const Mesh& rhs);
    friend bool operator!=(const Mesh& lhs, const Mesh& rhs);
};
}    // namespace asl
