//
// Created by qwerty on 22/08/2025.
//
#pragma once

#include "../Entity.hpp"
// std
#include <filesystem>
#include <vector>
//
//
namespace odin::components
{
struct Model
{
    std::filesystem::path filename;
    std::vector<Entity> submeshes;
};
}    // namespace odin::components
