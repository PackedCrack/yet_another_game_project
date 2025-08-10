//
// Created by qwerty on 22/07/2024.
//
#pragma once

// external
#include "tinygltf/tiny_gltf.h"
#include "glm_headers.hpp"
// internal
#include "ModelNode.hpp"
//
//
namespace asl
{
//[[nodiscard]] common::CGraph<Mesh> load_model(const std::filesystem::path& filename);
[[nodiscard]] common::CGraph<ModelNode> load_model(const std::filesystem::path& filename);
}    // namespace asl
