//
// Created by qwerty on 22/07/2024.
//
#pragma once

// external
#include "tinygltf/tiny_gltf.h"
// internal
#include "SceneGraphNode.hpp"
//
//
namespace asl
{
//[[nodiscard]] common::CGraph<Mesh> load_model(const std::filesystem::path& filename);
[[nodiscard]] common::CGraph<SceneGraphNode> load_model(const std::filesystem::path& filename);
}    // namespace asl
