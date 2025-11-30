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
#include <assetloader/UUID.hpp>
//
//
namespace odin::component
{
struct Model
{
    Model(asl::UUID uuid);
    asl::UUID uuid;
    asl::ModelHandle handle;
    Entity root;
    using Name = std::string;
    std::vector<std::pair<Name, Entity>> nodes;
};
}    // namespace odin::component
