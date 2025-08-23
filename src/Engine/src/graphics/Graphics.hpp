//
// Created by qwerty on 11/08/2025.
//
#pragma once

#include "../OdinInfo.hpp"
#include "../components/Model.hpp"
#include "MeshID.hpp"
// AssetLoader
#include <assetloader/Model.hpp>
//
//
namespace odin::graphics
{
class Graphics
{
    class Impl;
public:
    Graphics(const OdinInfo& info);
    ~Graphics();
    Graphics(Graphics&& other) noexcept;
    Graphics& operator=(Graphics&& other) noexcept;
public:
    void draw();
    void register_model(const asl::Model& sceneGraph);
    [[nodiscard]] bool is_registered(const components::Model& model) const;
    [[nodiscard]] std::vector<MeshID> mesh_ids(const components::Model& model) const;
private:
    std::unique_ptr<Impl> m_pImpl;
};
}    // namespace odin::graphics
