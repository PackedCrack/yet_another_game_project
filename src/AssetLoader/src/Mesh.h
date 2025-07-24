//
// Created by qwerty on 22/07/2024.
//

#pragma once

#include "CGraph.hpp"
#include "glm_headers.h"
#include "Material.h"
//
//
namespace asl
{
struct Primitive    // Mesh 2.0
{
    glm::vec3 translation;
    glm::quat rotation;
    glm::vec3 scale;

    std::vector<glm::vec3> position;    // Vertex positions
    std::vector<uint16_t> indices;
    std::optional<std::vector<glm::vec3>> normal;
    std::optional<std::vector<glm::vec4>> tanget;
    std::optional<std::vector<glm::vec2>> uv;
    std::optional<std::vector<glm::vec4>> color;
    std::optional<std::vector<glm::vec4>> joints;
    std::optional<std::vector<glm::vec4>> weights;

    // TODO:
    // GPU Topology mode goes here.

    // <key, value> = <COLOR_n, data>
    std::unordered_map<std::string, std::vector<glm::vec4>> ColorCache;
    // <key, value> = <TEXCOORD_n, data>
    std::unordered_map<std::string, std::vector<glm::vec2>> UVCache;
    // <key, value> = <JOINTS_n, data>
    std::unordered_map<std::string, std::vector<glm::vec4>> jointCache;
    // <key, value> = <WEIGHTS_n, data>
    std::unordered_map<std::string, std::vector<glm::vec4>> weightsCache;

    Material material;
};
struct Mesh : public common::GraphVertex<Mesh>
{
    Mesh();
    std::size_t counter;    // replace this with UUID
    std::size_t id;


    glm::vec3 translation;
    glm::quat rotation;
    glm::vec3 scale;


    std::vector<glm::vec3> position;    // Vertex positions
    std::vector<uint16_t> indices;
    std::optional<std::vector<glm::vec3>> normal;
    std::optional<std::vector<glm::vec4>> tanget;
    std::optional<std::vector<glm::vec2>> uv;
    std::optional<std::vector<glm::vec4>> color;
    std::optional<std::vector<glm::vec4>> joints;
    std::optional<std::vector<glm::vec4>> weights;


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
