//
// Created by qwerty on 24/07/2025.
//
#pragma once

#include "ModelHandle.hpp"
#include "UUID.hpp"
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
    using AssetMap = std::unordered_map<UUID, std::filesystem::path, UUIDHasher>;
public:
    AssetRegistry(const std::filesystem::path& path);
public:
    void reload(const std::filesystem::path& filepath);
    [[nodiscard]] ModelHandle model_handle(const std::filesystem::path& filepath);
private:
    [[nodiscard]] std::shared_ptr<ModelSlot> slot(const std::filesystem::path& filepath);
private:
    Registry m_SceneGraphs;
    AssetMap m_AssetMap;
    Mutex m_pMutex;

    // hashmap tha stores uuid:filepath
    // then in model_handle that calls slot() that retrieves filepath and try and emplace into SceneGraph
    // lägg till kod i reload och model_handle
};
}    // namespace asl
