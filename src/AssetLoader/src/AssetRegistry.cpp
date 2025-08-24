//
// Created by qwerty on 24/07/2025.
//
#include "AssetRegistry.hpp"

#include "SceneGraph.hpp"
//
//
namespace asl
{
void AssetRegistry::reload(const std::string& filename)
{
    std::shared_ptr<ModelSlot> pSlot = slot(filename);
    auto pNew = std::make_shared<SceneGraph>(filename);
    pSlot->pGraph.store(std::move(pNew));
}
ModelHandle AssetRegistry::model_handle(const std::string& filename)
{
    std::shared_ptr<ModelSlot> pSlot = slot(filename);

    std::shared_ptr<const SceneGraph> pGraph = pSlot->pGraph.load();
    if (pGraph == nullptr)
    {
        auto pModel = std::make_shared<SceneGraph>(filename);
        pSlot->pGraph.store(pModel);
    }

    return ModelHandle{ std::move(pSlot) };
}
std::shared_ptr<ModelSlot> AssetRegistry::slot(const std::string& filename)
{
    std::shared_ptr<ModelSlot> pSlot = nullptr;
    if (!m_SceneGraphs.contains(filename))
    {
        pSlot = std::make_shared<ModelSlot>();
        m_SceneGraphs.emplace(filename, pSlot->weak_from_this());
    }
    else
    {
        std::weak_ptr<ModelSlot> wpSlot = m_SceneGraphs.at(filename);
        pSlot = wpSlot.lock();
    }

    return pSlot;
}
}    // namespace asl
