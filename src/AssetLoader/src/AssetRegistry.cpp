//
// Created by qwerty on 24/07/2025.
//
#include "AssetRegistry.hpp"
#include "SceneGraph.hpp"
//
//
#include <iostream>
#include <filesystem>
namespace
{
std::unordered_map<asl::UUID, std::filesystem::path, asl::UUIDHasher> make_asset_map(const std::filesystem::path& path)
{
    std::unordered_map<asl::UUID, std::filesystem::path, asl::UUIDHasher> tmpAssetMap{};

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        assert(false);
        // FATAL ERROR
    }

    const char delimiter{ '\r' };
    std::string uuidLine{};
    std::string pathLine{};
    while (std::getline(file, uuidLine))
    {
        if (!uuidLine.empty())
        {
            if (uuidLine.back() == delimiter)
            {
                uuidLine.pop_back();
            }

            if (!std::getline(file, pathLine))
            {
                assert(false);
                // TODO handle error gracefully
                // assign UUID to debug model
            }
            if (pathLine.back() == delimiter)
            {
                pathLine.pop_back();
            }
            tmpAssetMap.emplace(asl::UUID{ uuidLine }, std::filesystem::path(pathLine));
        }
    }
    return tmpAssetMap;
}
}    // namespace
namespace asl
{
AssetRegistry::AssetRegistry(const std::filesystem::path& path)
    : m_SceneGraphs{}
    , m_AssetMap{ make_asset_map(path) }
    , m_pMutex{ std::make_unique<mutex_t>() }
{}
void AssetRegistry::reload(const UUID& uuid)
{
    std::shared_ptr<ModelSlot> pSlot = slot(uuid);
    std::filesystem::path& assetFilePath{ uuid_to_path(uuid) };
    auto pNew = std::make_shared<SceneGraph>(assetFilePath);
    std::atomic_store(std::addressof(pSlot->pGraph), std::move(pNew));
}
ModelHandle AssetRegistry::model_handle(const UUID& uuid)
{
    std::shared_ptr<ModelSlot> pSlot = slot(uuid);

    std::shared_ptr<const SceneGraph> pGraph = std::atomic_load(std::addressof(pSlot->pGraph));
    if (pGraph == nullptr)
    {
        std::lock_guard lock{ pSlot->initMutex };

        pGraph = std::atomic_load(std::addressof(pSlot->pGraph));
        if (pGraph == nullptr)
        {
            std::filesystem::path& assetFilePath{ uuid_to_path(uuid) };
            auto pModel = std::make_shared<const SceneGraph>(assetFilePath);
            std::atomic_store(std::addressof(pSlot->pGraph), std::move(pModel));
        }
    }

    return ModelHandle{ std::move(pSlot) };
}
// return std::optional instead
std::filesystem::path& AssetRegistry::uuid_to_path(const UUID& uuid)
{
    auto it = m_AssetMap.find(uuid);
    if (it != m_AssetMap.end())
    {
        return it->second;
    }
    else
    {
        assert(false);
        // Assert here for now, This SHOULD never happen. Because if a UUID is not found when
        // the AssetMap is created. We will load debug asset instead.
    }
}
std::shared_ptr<ModelSlot> AssetRegistry::slot(const UUID& uuid)
{
    std::filesystem::path& assetFilePath{ uuid_to_path(uuid) };
    std::lock_guard<mutex_t> lock(*m_pMutex);
    auto [kvPair, emplaced] = m_SceneGraphs.try_emplace(assetFilePath.string(), std::weak_ptr<ModelSlot>{});
    if (std::shared_ptr<ModelSlot> pSlot = kvPair->second.lock(); pSlot != nullptr)
    {
        return pSlot;
    }
    auto pSlot = std::make_shared<ModelSlot>();
    kvPair->second = pSlot->weak_from_this();

    return pSlot;
}
}    // namespace asl
