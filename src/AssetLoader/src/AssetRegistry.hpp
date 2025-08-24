//
// Created by qwerty on 24/07/2025.
//
#pragma once

#include "ModelHandle.hpp"
// std
#include <memory>
#include <string>
#include <unordered_map>
//
//
namespace asl
{
class AssetRegistry
{
    using Registry = std::unordered_map<std::string, std::weak_ptr<ModelSlot>>;
public:
    void reload(const std::string& filename);
    [[nodiscard]] ModelHandle model_handle(const std::string& filename);
private:
    [[nodiscard]] std::shared_ptr<ModelSlot> slot(const std::string& filename);
private:
    Registry m_SceneGraphs;
};
}    // namespace asl
