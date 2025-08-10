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
private:
    std::filesystem::path m_Filename;
    common::CGraph<ModelNode> m_Model;
};
///////////////
// Interface //
Model::Model(std::filesystem::path filename)
    : m_pImpl{ std::make_unique<Impl>(std::move(filename)) }
{}
Model::~Model() = default;
Model::Model(Model&&) noexcept = default;
Model& Model::operator=(Model&&) noexcept = default;
}    // namespace asl
