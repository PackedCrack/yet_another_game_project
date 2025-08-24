//
// Created by qwerty on 24/07/2025.
//
#include "AssetRegistry.hpp"

#include "SceneGraph.hpp"
//
//
namespace asl
{
AssetRegistry::AssetRegistry()
    : m_SceneGraphs{}
    , m_pMutex{ std::make_unique<mutex_t>() }
{}
void AssetRegistry::reload(const std::filesystem::path& filepath)
{
    std::shared_ptr<ModelSlot> pSlot = slot(filepath);
    auto pNew = std::make_shared<SceneGraph>(filepath);
    std::atomic_store(std::addressof(pSlot->pGraph), std::move(pNew));
}
ModelHandle AssetRegistry::model_handle(const std::filesystem::path& filepath)
{
    std::shared_ptr<ModelSlot> pSlot = slot(filepath);

    std::shared_ptr<const SceneGraph> pGraph = std::atomic_load(std::addressof(pSlot->pGraph));
    if (pGraph == nullptr)
    {
        std::lock_guard lock{ pSlot->initMutex };

        pGraph = std::atomic_load(std::addressof(pSlot->pGraph));
        if (pGraph == nullptr)
        {
            auto pModel = std::make_shared<const SceneGraph>(filepath);
            std::atomic_store(std::addressof(pSlot->pGraph), std::move(pModel));
        }
    }

    return ModelHandle{ std::move(pSlot) };
}
std::shared_ptr<ModelSlot> AssetRegistry::slot(const std::filesystem::path& filepath)
{
    std::string fp = filepath.string();

    std::lock_guard<mutex_t> lock(*m_pMutex);
    auto [kvPair, emplaced] = m_SceneGraphs.try_emplace(fp, std::weak_ptr<ModelSlot>{});
    if (std::shared_ptr<ModelSlot> pSlot = kvPair->second.lock(); pSlot != nullptr)
    {
        return pSlot;
    }

    auto pSlot = std::make_shared<ModelSlot>();
    kvPair->second = pSlot->weak_from_this();

    return pSlot;
}
}    // namespace asl
