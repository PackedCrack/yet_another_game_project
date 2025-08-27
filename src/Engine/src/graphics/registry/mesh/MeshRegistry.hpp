//
// Created by qwerty on 20/08/2025.
//
#pragma once

#include "MeshID.hpp"
#include "../../gpu_types.hpp"
#include "../../Renderer.hpp"
#include "../../TransferManager.hpp"
#include "../../vk/Allocator.hpp"
#include "../../vk/QueueFamilies.hpp"
#include "../../../ArenaAllocator.hpp"
// AssetLoader
#include <assetloader/ModelHandle.hpp>
//
//
namespace odin::graphics::registry::mesh
{
struct EntryAllocation
{
    ArenaAllocation vertices;
    ArenaAllocation indices;
    ArenaAllocation meshTable;
};
struct MeshEntry
{
    static constexpr MeshID DUMMY_ID = std::numeric_limits<MeshID>::max();
    MeshID id;
    std::uint64_t lastUsed;
    std::optional<EntryAllocation> allocation;
};
class MeshRegistry
{
public:
    // Entries are stored in DFS order
    using Registry = std::unordered_map<std::string, std::vector<MeshEntry>>;    // should be UUID
    using vertex_t = vk::resource::Vertex;
    using index_t = vk::resource::IndexBuffer::index_t;

    MeshRegistry(const RenderResources& renderResources);
public:
    void touch(const asl::ModelHandle& handle);
    void register_model(TransferManager& transferManager,
                        const RenderResources& renderResources,
                        vk::QueueView graphicsQ,
                        const std::shared_ptr<vk::Allocator>& pAllocator,
                        const asl::ModelHandle& handle);
    [[nodiscard]] const std::vector<MeshEntry>& entries(const asl::ModelHandle& handle) const;
    [[nodiscard]] bool contains(const asl::ModelHandle& handle) const;
private:
    [[nodiscard]] std::optional<EntryAllocation> make_entry_allocation(std::span<vertex_t> vertices, std::span<const index_t> indices);
    [[nodiscard]] MeshEntry make_entry(std::span<vertex_t> vertices, std::span<const index_t> indices);
private:
    std::unique_ptr<ArenaAllocator<vertex_t>> m_pVertexArena;
    std::unique_ptr<ArenaAllocator<index_t>> m_pIndexArena;
    std::unique_ptr<ArenaAllocator<MeshInfo>> m_pMeshTableArena;
    Registry m_Meshes;
};
}    // namespace odin::graphics::registry::mesh
