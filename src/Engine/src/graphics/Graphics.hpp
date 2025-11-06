//
// Created by qwerty on 11/08/2025.
//
#pragma once

#include "../OdinInfo.hpp"
#include "../window/Window.hpp"
#include "gpu_types.hpp"    // Leaking GPU types.. is this a problem?
#include "registry/mesh/MeshID.hpp"
// AssetLoader
#include <assetloader/ModelHandle.hpp>
//
//
namespace odin::graphics
{
class Graphics
{
    class Impl;
public:
    Graphics(const OdinInfo& info, window::Window& window);
    ~Graphics();
    Graphics(Graphics&& other) noexcept;
    Graphics& operator=(Graphics&& other) noexcept;
public:
    void draw(std::span<const InstanceInfo> instanceInfos);
    void register_model(const asl::ModelHandle& handle);
    [[nodiscard]] bool is_registered(const asl::ModelHandle& handle) const;
    [[nodiscard]] std::vector<registry::mesh::MeshID> mesh_ids(const asl::ModelHandle& handle) const;
private:
    std::unique_ptr<Impl> m_pImpl;
};
}    // namespace odin::graphics
