//
// Created by qwerty on 24/07/2025.
//

#pragma once

#include "common_types.hpp"
#include "Material.hpp"
// glm
#include <glm_headers.hpp>
// std
#include <optional>
#include <string>
#include <vector>
#include <unordered_map>
//
//
namespace asl
{
//enum class PrimitiveMode
//{
//    POINTS = TINYGLTF_MODE_POINTS,
//    LINE = TINYGLTF_MODE_LINE,
//    LINE_LOOP = TINYGLTF_MODE_LINE_LOOP,
//    LINE_STRIP = TINYGLTF_MODE_LINE_STRIP,
//    TRIANGLES = TINYGLTF_MODE_TRIANGLES,
//    TRIANGLE_STRIP = TINYGLTF_MODE_TRIANGLE_STRIP,
//    TRIANGLE_FAN = TINYGLTF_MODE_TRIANGLE_FAN,
//    // if something goes wrong
//    UNKNOWN
//};
//
//
//
struct Renderable
{
    using color_cache = std::unordered_map<std::string, std::vector<glm::vec4>>;
    using uv_cache = std::unordered_map<std::string, std::vector<glm::vec2>>;
    using joints_cache = std::unordered_map<std::string, std::vector<glm::vec4>>;
    using weights_cache = std::unordered_map<std::string, std::vector<glm::vec4>>;

    std::vector<glm::vec3> vertexPosition;
    std::vector<std::uint16_t> indices;
    std::optional<std::vector<glm::vec3>> normal;
    std::optional<std::vector<glm::vec4>> tangent;

    PrimitiveMode topology;

    // <key, value> = <TEXCOORD_n, data>
    uv_cache textureCoordinates;
    // <key, value> = <COLOR_n, data>
    color_cache colors;
    // <key, value> = <JOINTS_n, data>
    joints_cache joints;
    // <key, value> = <WEIGHTS_n, data>
    weights_cache weights;

    Material material;
};
struct Mesh
{
    [[nodiscard]] MeshView view() const;
    std::string name;
    std::vector<Renderable> renderables;
};
[[nodiscard]] std::optional<Mesh> make_mesh(const tinygltf::Model& model, const tinygltf::Node& node);
}    // namespace asl
