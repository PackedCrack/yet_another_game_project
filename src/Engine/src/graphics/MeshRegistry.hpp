//
// Created by qwerty on 20/08/2025.
//
#pragma once

#include "../ArenaAllocator.hpp"
#include "gpu_types.hpp"
#include "Renderer.hpp"
#include "TransferManager.hpp"
#include "vk/Allocator.hpp"
#include "vk/QueueFamilies.hpp"
#include "vk/resource/VertexBuffer.hpp"
// AssetLoader
#include <assetloader/Model.hpp>
//
//
namespace odin::graphics
{
using MeshID = std::int32_t;
class MeshRegistry
{
    struct MeshEntry
    {
        MeshID id;
        ArenaAllocation allocation;
        std::uint64_t lastUsed;
    };
public:
    using Meshes = std::unordered_map<std::string, MeshEntry>;    // should be UUID
    using vertex_t = vk::resource::Vertex;
    using index_t = vk::resource::Index;

    MeshRegistry(const RenderResources& renderResources);
public:
    void load_model(TransferManager& transferManager,
                    vk::QueueView graphicsQ,
                    const std::shared_ptr<vk::Allocator>& pAllocator,
                    const asl::Model& model);
    [[nodiscard]] MeshID get_mesh(const std::string& filepath);
    [[nodiscard]] bool contains(const std::string& filepath);
private:
    [[nodiscard]] MeshID touch_and_return(const std::string& filepath);
private:
    Meshes m_Meshes;
    std::unique_ptr<ArenaAllocator<vertex_t>> m_pVertexArena;
    std::unique_ptr<ArenaAllocator<index_t>> m_pIndexArena;
    std::unique_ptr<ArenaAllocator<MeshInfo>> m_pMeshTableArena;
};
}    // namespace odin::graphics
