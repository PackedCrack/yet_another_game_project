//
// Created by qwerty on 22/07/2024.
//
#pragma once
#include <memory>
//
//
namespace assloader
{
class ModelDataImpl;
class ModelData
{
public:
    ModelData();
private:
    std::unique_ptr<ModelDataImpl> m_pModelData = nullptr;
};
}   // namespace assloader