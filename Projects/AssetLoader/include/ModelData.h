//
// Created by qwerty on 22/07/2024.
//
#pragma once

#include <memory>
#include <filesystem>
//
//
namespace asl
{
class ModelDataImpl;
class ModelData
{
public:
    explicit ModelData(std::filesystem::path filename);
private:
    std::unique_ptr<ModelDataImpl> m_pModelData;
};
}    // namespace asl
