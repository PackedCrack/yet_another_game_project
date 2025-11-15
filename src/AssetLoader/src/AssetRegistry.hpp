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
    void reload(const UUID& uuid);
    [[nodiscard]] ModelHandle model_handle(const UUID& uuid);
    [[nodiscard]] std::filesystem::path& uuid_to_path(const UUID& uuid);
private:
    [[nodiscard]] std::shared_ptr<ModelSlot> slot(const UUID& uuid);
private:
    Registry m_SceneGraphs;
    AssetMap m_AssetMap;
    Mutex m_pMutex;
};
}    // namespace asl
