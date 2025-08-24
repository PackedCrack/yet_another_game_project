//
// Created by qwerty on 22/08/2025.
//
#pragma once

#include "../Entity.hpp"
// std
#include <filesystem>
#include <vector>
// assetloader
#include <assetloader/ModelHandle.hpp>
//
//
namespace odin::component
{
struct Model
{
    Model(std::filesystem::path path);
    std::filesystem::path filepath;
    asl::ModelHandle handle;
    std::vector<Entity> submeshes;
};
}    // namespace odin::component
