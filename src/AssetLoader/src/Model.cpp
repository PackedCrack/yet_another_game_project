//
// Created by qwerty on 23/07/2024.
//
#include "Model.hpp"
#include "ModelNode.hpp"
#include "gltf_loader.hpp"
#include "asl_defines.hpp"
//
//
namespace asl
{
class Model::Impl
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
    common::CGraph<ModelNode> m_Model;
};
///////////////////
// Exposed Pimpl //
Model::Model(std::filesystem::path filename)
    : m_pImpl{ std::make_unique<Impl>(std::move(filename)) }
{}
Model::~Model() = default;
Model::Model(Model&&) noexcept = default;
Model& Model::operator=(Model&&) noexcept = default;
//std::vector<RenderableView> Model::view_renderables() const
//{
//    return m_pImpl->view_renderables();
//}
void Model::dfs(std::function<void(const NodeView*, const NodeView*)> visitor) const
{
    std::unordered_map<const ModelNode*, NodeView> viewCache{};
    auto implVisitor = [&viewCache, v = std::move(visitor)](const ModelNode* pParent, const ModelNode* pChild)
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
std::string Model::filename() const
{
    return m_pImpl->filename();
}
}    // namespace asl
