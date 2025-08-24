//
// Created by qwerty on 23/07/2024.
//
#include "SceneGraph.hpp"
#include "SceneGraphNode.hpp"
#include "gltf_loader.hpp"
#include "asl_defines.hpp"
//
//
namespace asl
{
class SceneGraph::Impl
{
public:
    explicit Impl(std::filesystem::path&& filename)
        : m_Filename{ std::move(filename) }
        , m_Model{ load_model(m_Filename) }
    {}
    template<typename invocable_t>
    void dfs(invocable_t&& invocable) const
    {
        m_Model.dfs(std::forward<invocable_t>(invocable));
    }
    std::string filename() const { return m_Filename.string(); }
private:
    std::filesystem::path m_Filename;
    common::CGraph<SceneGraphNode> m_Model;
};
///////////////////
// Exposed Pimpl //
SceneGraph::SceneGraph(std::filesystem::path filename)
    : m_pImpl{ std::make_unique<Impl>(std::move(filename)) }
{}
SceneGraph::~SceneGraph() = default;
SceneGraph::SceneGraph(SceneGraph&&) noexcept = default;
SceneGraph& SceneGraph::operator=(SceneGraph&&) noexcept = default;
void SceneGraph::dfs(std::function<void(const NodeView*, const NodeView*)> visitor) const
{
    std::unordered_map<const SceneGraphNode*, NodeView> viewCache{};
    auto implVisitor = [&viewCache, v = std::move(visitor)](const SceneGraphNode* pParent, const SceneGraphNode* pChild)
    {
        const NodeView* pParentView = nullptr;
        if (pParent != nullptr)
        {
            if (!viewCache.contains(pParent))
            {
                viewCache.emplace(pParent, pParent->view());
            }
            pParentView = std::addressof(viewCache.at(pParent));
        }
        if (!viewCache.contains(pChild))
        {
            viewCache.emplace(pChild, pChild->view());
        }

        const NodeView* pChildView = std::addressof(viewCache.at(pChild));
        v(pParentView, pChildView);
    };
    m_pImpl->dfs(std::move(implVisitor));
}
std::string SceneGraph::filename() const
{
    return m_pImpl->filename();
}
}    // namespace asl
