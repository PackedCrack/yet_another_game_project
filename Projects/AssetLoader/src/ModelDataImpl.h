//
// Created by qwerty on 22/07/2024.
//
#pragma once

#include "ModelData.h"
#include "Mesh.h"
//
//
namespace asl
{
class ModelDataImpl
{
public:
    explicit ModelDataImpl(std::filesystem::path&& filename);
private:
    std::filesystem::path m_Filename;
    common::CGraph<Mesh> m_Meshes;
};
}    // namespace asl
