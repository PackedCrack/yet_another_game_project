//
// Created by qwerty on 20/08/2025.
//
#include "MeshRegistry.hpp"

// common
#include <common.hpp>
//
//
namespace
{
using Vertex = odin::graphics::registry::mesh::MeshRegistry::vertex_t;
using Index = odin::graphics::registry::mesh::MeshRegistry::index_t;
using EntryAllocation = odin::graphics::registry::mesh::EntryAllocation;
using MeshEntry = odin::graphics::registry::mesh::MeshEntry;
using MeshInfo = odin::graphics::MeshInfo;
using MeshTableArena = odin::ArenaAllocator<MeshInfo>;
using VertexArena = odin::ArenaAllocator<Vertex>;
using IndexArena = odin::ArenaAllocator<Index>;
using ArenaAllocation = odin::ArenaAllocation;
using RenderResources = odin::graphics::RenderResources;
using TransferManager = odin::graphics::TransferManager;
using Allocator = odin::graphics::vk::Allocator;
using QueueView = odin::graphics::vk::QueueView;
using BufferRef = odin::graphics::vk::resource::BufferRef;
using StagingBuffer = odin::graphics::vk::resource::StagingBuffer;
//
//
template<typename buffer_t>
requires std::ranges::contiguous_range<buffer_t>
void upload_to_gpu(TransferManager& transferManager,
                   const std::shared_ptr<Allocator>& pAllocator,
                   BufferRef dst,
                   const QueueView& queue,
                   const ArenaAllocation& allocation,
                   buffer_t&& content)
{
    std::unique_ptr<StagingBuffer> pStaging = pAllocator->to_staging_buffer(std::forward<buffer_t>(content));
    transferManager.enqueue_buffer_transfer(dst, std::move(pStaging), queue, allocation);
}
[[nodiscard]] std::unique_ptr<MeshTableArena> make_mesh_table_arena(const RenderResources& renderResources)
{
    std::size_t elementCapacity = renderResources.meshTable.byte_capacity() / sizeof(MeshInfo);
    VkDeviceSize minAlignment = renderResources.meshTable.min_alignment();

    return MeshTableArena::make_arena_allocator(elementCapacity, minAlignment);
}
[[nodiscard]] std::unique_ptr<VertexArena> make_vertex_arena(const RenderResources& renderResources)
{
    std::size_t elementCapacity = renderResources.vertexBuffer.capacity();
    VkDeviceSize minAlignment = renderResources.vertexBuffer.min_alignment();

    return VertexArena::make_arena_allocator(elementCapacity, minAlignment);
}
[[nodiscard]] std::unique_ptr<IndexArena> make_index_arena(const RenderResources& renderResources)
{
    std::size_t elementCapacity = renderResources.indexBuffer.capacity();
    VkDeviceSize minAlignment = renderResources.indexBuffer.min_alignment();

    return IndexArena::make_arena_allocator(elementCapacity, minAlignment);
}
[[nodiscard]] std::uint32_t make_draw_flags(const asl::RenderableView& rv)
{
    std::uint32_t drawFlags{};
    drawFlags |= rv.normal ? DRAW_FLAG_HAS_NORMAL : 0;
    drawFlags |= rv.tangent ? DRAW_FLAG_HAS_TANGENT : 0;
    drawFlags |= rv.texcoord_0 ? DRAW_FLAG_HAS_TEXCOORD_0 : 0;
    drawFlags |= rv.texcoord_1 ? DRAW_FLAG_HAS_TEXCOORD_1 : 0;

    return drawFlags;
}
[[nodiscard]] MeshInfo make_mesh_info(const asl::RenderableView& rv, std::span<const Index> indices, const MeshEntry& entry)
{
    const EntryAllocation& allocation = entry.allocation.value();
    return MeshInfo{ .indexCount = static_cast<std::uint32_t>(indices.size()),
                     .firstIndex = static_cast<std::uint32_t>(allocation.indices.element_offset()),
                     .vertexOffset = static_cast<std::int32_t>(allocation.vertices.element_offset()),
                     // TODO:
                     .materialID = std::numeric_limits<std::uint32_t>::max(),
                     .drawFlags = make_draw_flags(rv) };
}
[[nodiscard]] std::vector<Vertex> make_interleaved_vertices(const asl::RenderableView& rv)
{
    using Vec2View = std::span<const glm::vec2>;
    using Vec3View = std::span<const glm::vec3>;
    using Vec4View = std::span<const glm::vec4>;

    std::vector<Vertex> vertices{};
    const Vec3View& vertexPositions = rv.vertexPosition;
    for (std::size_t i = 0; i < vertexPositions.size(); ++i)
    {
        vertices.emplace_back();
        Vertex& vertex = vertices.back();

        vertex.position = vertexPositions[i];
        if (rv.normal)
        {
            const Vec3View& normals = rv.normal.value();
            vertex.normal = normals[i];
        }
        if (rv.tangent)
        {
            const Vec4View& tangents = rv.tangent.value();
            vertex.tangent = tangents[i];
        }
        if (rv.texcoord_0)
        {
            const Vec2View& texcoord_0 = rv.texcoord_0.value();
            vertex.texcoord_0 = texcoord_0[i];
        }
        if (rv.texcoord_1)
        {
            const Vec2View& texcoord_1 = rv.texcoord_1.value();
            vertex.texcoord_1 = texcoord_1[i];
        }
    }

    return vertices;
}
[[nodiscard]] MeshEntry make_dummy_entry()
{
    return MeshEntry{ .id = MeshEntry::DUMMY_ID, .lastUsed = 0, .allocation = std::nullopt };
}
}    // namespace
namespace odin::graphics::registry::mesh
{
MeshRegistry::MeshRegistry(const RenderResources& renderResources)
    : m_pVertexArena{ make_vertex_arena(renderResources) }    //, m_IndexArena{ make_index_arena(renderResources) }
    , m_pIndexArena{ make_index_arena(renderResources) }
    , m_pMeshTableArena{ make_mesh_table_arena(renderResources) }
    , m_Meshes{}
{}
void MeshRegistry::touch(const asl::ModelHandle& handle)
{
    std::shared_ptr<const asl::SceneGraph> pGraph = handle.acquire();
    std::string filename = pGraph->filename().string();
    ODIN_ASSERT(m_Meshes.contains(filename));

    std::vector<MeshEntry>& meshEntries = m_Meshes.at(filename);
    for (auto&& mesh : meshEntries)
    {
        mesh.lastUsed++;
    }
}
void MeshRegistry::register_model(TransferManager& transferManager,
                                  const RenderResources& resources,
                                  vk::QueueView graphicsQ,
                                  const std::shared_ptr<vk::Allocator>& pAllocator,
                                  const asl::ModelHandle& handle)
{
    std::shared_ptr<const asl::SceneGraph> pGraph = handle.acquire();

    auto [kvPair, emplaced] = m_Meshes.try_emplace(pGraph->filename().string());
    ODIN_ASSERT(emplaced);

    std::vector<MeshEntry>& newEntries = kvPair->second;
    auto visitor = [&]([[maybe_unused]] const asl::NodeView* pParent, const asl::NodeView* pChild)
    {
        if (pChild->geometry)
        {
            std::vector<asl::RenderableView> views = pChild->geometry->renderables;
            for (auto&& rv : views)
            {
                std::vector<vertex_t> v = make_interleaved_vertices(rv);

                std::span<vertex_t> vertices = common::to_span(v);
                std::span<const index_t> indices = rv.indices;
                MeshEntry entry = make_entry(vertices, indices);

                const EntryAllocation& ea = entry.allocation.value();

                BufferRef vertexBuffer = resources.vertexBuffer.handle();
                upload_to_gpu(transferManager, pAllocator, vertexBuffer, graphicsQ, ea.vertices, vertices);

                BufferRef indexBuffer = resources.indexBuffer.handle();
                upload_to_gpu(transferManager, pAllocator, indexBuffer, graphicsQ, ea.indices, indices);

                // hack to get the .data() and .size() members..
                std::array<MeshInfo, 1> meshInfo{};
                meshInfo[0] = make_mesh_info(rv, indices, entry);
                BufferRef meshTable = resources.meshTable.handle();
                upload_to_gpu(transferManager, pAllocator, meshTable, graphicsQ, ea.meshTable, common::to_span(meshInfo));


                newEntries.push_back(std::move(entry));
            }
        }
        else
        {
            newEntries.push_back(make_dummy_entry());
        }
    };
    pGraph->dfs(std::move(visitor));
}
const std::vector<MeshEntry>& MeshRegistry::entries(const asl::ModelHandle& handle) const
{
    std::shared_ptr<const asl::SceneGraph> pGraph = handle.acquire();
    // TODO: UUID in the future
    return m_Meshes.at(pGraph->filename().string());
}
bool MeshRegistry::contains(const asl::ModelHandle& handle) const
{
    std::shared_ptr<const asl::SceneGraph> pGraph = handle.acquire();
    // TODO: UUID in the future
    return m_Meshes.contains(pGraph->filename().string());
}
std::optional<EntryAllocation> MeshRegistry::make_entry_allocation(std::span<vertex_t> vertices, std::span<const index_t> indices)
{
    return EntryAllocation{
        .vertices = m_pVertexArena->insert_range(vertices),
        .indices = m_pIndexArena->insert_range(indices),
        .meshTable = m_pMeshTableArena->insert(),
    };
}
MeshEntry MeshRegistry::make_entry(std::span<vertex_t> vertices, std::span<const index_t> indices)
{
    MeshEntry entry{ .allocation = make_entry_allocation(vertices, indices) };

    const ArenaAllocation& meshTable = entry.allocation->meshTable;
    entry.id = static_cast<std::uint32_t>(meshTable.element_offset());
    entry.lastUsed = 0;

    return entry;
}
}    // namespace odin::graphics::registry::mesh
