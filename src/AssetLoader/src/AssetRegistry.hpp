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
    using mutex_t = std::mutex;
    using Mutex = std::unique_ptr<mutex_t>;
    using Registry = std::unordered_map<std::string, std::weak_ptr<ModelSlot>>;
public:
    AssetRegistry();
public:
    void reload(const std::filesystem::path& filepath);
    [[nodiscard]] ModelHandle model_handle(const std::filesystem::path& filepath);
private:
    [[nodiscard]] std::shared_ptr<ModelSlot> slot(const std::filesystem::path& filepath);
private:
    Registry m_SceneGraphs;
    Mutex m_pMutex;
};
}    // namespace asl
