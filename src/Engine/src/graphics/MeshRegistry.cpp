//
// Created by qwerty on 20/08/2025.
//
#include "MeshRegistry.hpp"
//
//
namespace
{
using MeshTableArena = odin::ArenaAllocator<odin::graphics::MeshInfo>;
using VertexArena = odin::ArenaAllocator<odin::graphics::MeshRegistry::vertex_t>;
using IndexArena = odin::ArenaAllocator<odin::graphics::MeshRegistry::index_t>;
using namespace odin;
using namespace odin::graphics;
//
//
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
//[[nodiscard]] std::unique_ptr<IndexArena> make_index_arena(const RenderResources& renderResources)
//{
//    std::size_t elementCapacity = renderResources.indexBuffer.capacity();
//    VkDeviceSize minAlignment = renderResources.indexBuffer.min_alignment();
//
//    return IndexArena::make_arena_allocator(elementCapacity, minAlignment);
//}
}    // namespace
namespace odin::graphics
{
MeshRegistry::MeshRegistry(const RenderResources& renderResources)
    : m_Meshes{}
    , m_pVertexArena{ make_vertex_arena(renderResources) }    //, m_IndexArena{ make_index_arena(renderResources) }
    , m_pMeshTableArena{ make_mesh_table_arena(renderResources) }
{}
void MeshRegistry::load_model(TransferManager& transferManager,
                              vk::QueueView graphicsQ,
                              const std::shared_ptr<vk::Allocator>& pAllocator,
                              const asl::Model& model)
{
    // Model is a scenegraph - Somehow compact it internally to single meshes
    // std::vector<mesh> meshes = model.get_meshes();
    //
    // Take RenderResources as parameter
    // BufferRef vertexBuffer = renderResources.vertex_buffer();
    //
    std::vector<vertex_t> vertices{};    // Get this from meshes.vertices();
    ArenaAllocation vertexAlloc = m_pVertexArena->insert_range(vertices);
    BufferTransfer vertexTransfer{};
    // vertexTransfer.dstBuffer = vertexBuffer.handle
    vertexTransfer.dstOffset = vertexAlloc.element_offset();
    vertexTransfer.ownerQ = graphicsQ;
    vertexTransfer.pSrcBuffer =
        std::make_unique<vk::resource::StagingBuffer>(pAllocator->create_staging_buffer(vertices.size(), sizeof(vertex_t)));
    vertexTransfer.size = vertices.size() * sizeof(vertex_t);    // Should probably be able to calculate this inside TransferManager

    transferManager.enqueue_buffer_transfer(std::move(vertexTransfer));

    // Upload Indices
    // Upload MeshInfo to MeshTable
    // Upload Materials elsewhere?
}
MeshID MeshRegistry::get_mesh(const std::string& filepath)
{
    ODIN_ASSERT(m_Meshes.contains(filepath));
    return touch_and_return(filepath);
}
bool MeshRegistry::contains(const std::string& filepath)
{
    return m_Meshes.contains(filepath);
}
MeshID MeshRegistry::touch_and_return(const std::string& filepath)
{
    MeshEntry& mesh = m_Meshes.at(filepath);
    mesh.lastUsed++;
    return mesh.id;
}
}    // namespace odin::graphics
