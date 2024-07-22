//
// Created by qwerty on 22/07/2024.
//
#include "ModelDataImpl.h"
#include "gltf_loader.h"
//
//
namespace asl
{
ModelData::ModelData(std::filesystem::path filename)
    : m_pModelData{ std::make_unique<ModelDataImpl>(std::move(filename)) }
{}
ModelDataImpl::ModelDataImpl(std::filesystem::path&& filename)
    : m_Filename{ std::move(filename) }
    , m_Meshes{}
{
    m_Meshes = load_model(m_Filename);
}
}    // namespace asl
