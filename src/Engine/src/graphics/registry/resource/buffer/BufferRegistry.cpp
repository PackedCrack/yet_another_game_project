//
// Created by qwerty on 07/09/2025.
//
#include "BufferRegistry.hpp"

#include "../ResourceRegistry.hpp"
#include "../../../gpu_types.hpp"
// debug
#include <debug/debug_defines.hpp>
#include <debug/Logger.hpp>
//
//
namespace
{
using namespace odin::graphics;
using namespace odin::graphics::registry::resource::buffer;
//
//
[[nodiscard]] std::unique_ptr<vk::resource::IndexBuffer> make_index_buffer(const std::shared_ptr<vk::Allocator>& pAllocator)
{
    static constexpr std::uint64_t indexCapacity = 512 * 128 * 128;    // Aproximately 8,3 million indices

    vk::resource::IndexBuffer indexBuffer = pAllocator->create_index_buffer(indexCapacity);
    LOG_INFO("Creating Vertex Buffer with size: {} (bytes)", indexBuffer.byte_capacity());
    return std::make_unique<vk::resource::IndexBuffer>(std::move(indexBuffer));
}
[[nodiscard]] std::unique_ptr<vk::resource::VertexBuffer> make_vertex_buffer(const std::shared_ptr<vk::Allocator>& pAllocator)
{
    static constexpr std::uint64_t vertexCapacity = 512 * 128 * 128;    // Aproximately 8,3 million vertices

    vk::resource::VertexBuffer vertexBuffer = pAllocator->create_vertex_buffer(vertexCapacity);
    LOG_INFO("Creating Vertex Buffer with size: {} (bytes)", vertexBuffer.byte_capacity());
    return std::make_unique<vk::resource::VertexBuffer>(std::move(vertexBuffer));
}
[[nodiscard]] std::shared_ptr<vk::resource::StorageBuffer> make_mesh_table(const std::shared_ptr<vk::Allocator>& pAllocator)
{
    static constexpr std::uint64_t meshTableSize = 15000 * sizeof(odin::graphics::MeshInfo);

    vk::resource::StorageBuffer meshTable = pAllocator->create_storage_buffer(meshTableSize);
    LOG_INFO("Creating Storage Buffer (Mesh Table) with size: {} (bytes)", meshTable.byte_capacity());
    return std::make_shared<vk::resource::StorageBuffer>(std::move(meshTable));
}
[[nodiscard]] std::shared_ptr<vk::resource::StorageBuffer> make_material_table(const std::shared_ptr<vk::Allocator>& pAllocator)
{
    static constexpr std::uint64_t materialTableSize = 1000 * sizeof(odin::graphics::Material);

    vk::resource::StorageBuffer materialTable = pAllocator->create_storage_buffer(materialTableSize);
    LOG_INFO("Creating Storage Buffer (Material Table) with size: {} (bytes)", materialTable.byte_capacity());
    return std::make_shared<vk::resource::StorageBuffer>(std::move(materialTable));
}
[[nodiscard]] std::shared_ptr<vk::resource::DynamicStorageBuffer> make_draw_count_table(const std::shared_ptr<vk::Allocator>& pAllocator,
                                                                                        std::int32_t numFramesInFlight)
{
    VkDeviceSize partitionSize = sizeof(std::uint32_t);
    VkDeviceSize numPartitions = numFramesInFlight;
    bool transferDestination = true;
    bool indirectUsage = true;

    vk::resource::DynamicStorageBuffer drawCountTable =
        pAllocator->create_dynamic_storage_buffer(partitionSize, numPartitions, transferDestination, indirectUsage);
    LOG_INFO("Creating Dynamic Storage Buffer (Draw Count Table) with size: {} (bytes)", drawCountTable.byte_capacity());
    return std::make_shared<vk::resource::DynamicStorageBuffer>(std::move(drawCountTable));
}
[[nodiscard]] std::shared_ptr<vk::resource::DynamicStorageBuffer>
make_draw_args_table(const std::shared_ptr<vk::Allocator>& pAllocator, std::int32_t numFramesInFlight, std::uint32_t maxDraws)
{
    VkDeviceSize partitionSize = sizeof(VkDrawIndexedIndirectCommand) * maxDraws;
    VkDeviceSize numPartitions = numFramesInFlight;
    bool transferDestination = false;
    bool indirectUsage = true;

    vk::resource::DynamicStorageBuffer drawArgsTable =
        pAllocator->create_dynamic_storage_buffer(partitionSize, numPartitions, transferDestination, indirectUsage);
    LOG_INFO("Creating Dynamic Storage Buffer (Draw Arguments Table) with size: {} (bytes)", drawArgsTable.byte_capacity());
    return std::make_shared<vk::resource::DynamicStorageBuffer>(std::move(drawArgsTable));
}
[[nodiscard]] std::shared_ptr<vk::resource::DynamicStorageBuffer>
make_instance_base_table(const std::shared_ptr<vk::Allocator>& pAllocator, std::int32_t numFramesInFlight, std::uint32_t maxDraws)
{
    VkDeviceSize partitionSize = sizeof(std::uint32_t) * maxDraws;
    VkDeviceSize numPartitions = numFramesInFlight;
    bool transferDestination = false;
    bool indirectUsage = false;

    vk::resource::DynamicStorageBuffer instanceBaseTable =
        pAllocator->create_dynamic_storage_buffer(partitionSize, numPartitions, transferDestination, indirectUsage);
    LOG_INFO("Creating Dynamic Storage Buffer (Instance Base Table) with size: {} (bytes)", instanceBaseTable.byte_capacity());
    return std::make_shared<vk::resource::DynamicStorageBuffer>(std::move(instanceBaseTable));
}
[[nodiscard]] std::shared_ptr<vk::resource::DynamicStorageBuffer>
make_instance_counter_table(const std::shared_ptr<vk::Allocator>& pAllocator, std::int32_t numFramesInFlight, std::uint32_t maxDraws)
{
    VkDeviceSize partitionSize = sizeof(std::uint32_t) * maxDraws;
    VkDeviceSize numPartitions = numFramesInFlight;
    bool transferDestination = false;
    bool indirectUsage = false;

    vk::resource::DynamicStorageBuffer instanceCounterTable =
        pAllocator->create_dynamic_storage_buffer(partitionSize, numPartitions, transferDestination, indirectUsage);
    LOG_INFO("Creating Dynamic Storage Buffer (Instance Counter Table) with size: {} (bytes)", instanceCounterTable.byte_capacity());
    return std::make_shared<vk::resource::DynamicStorageBuffer>(std::move(instanceCounterTable));
}
[[nodiscard]] std::shared_ptr<vk::resource::DynamicStorageBuffer>
make_instance_index_table(const std::shared_ptr<vk::Allocator>& pAllocator, std::int32_t numFramesInFlight, std::uint32_t maxInstances)
{
    VkDeviceSize partitionSize = sizeof(std::uint32_t) * maxInstances;
    VkDeviceSize numPartitions = numFramesInFlight;
    bool transferDestination = false;
    bool indirectUsage = false;

    vk::resource::DynamicStorageBuffer instanceIndexTable =
        pAllocator->create_dynamic_storage_buffer(partitionSize, numPartitions, transferDestination, indirectUsage);
    LOG_INFO("Creating Dynamic Storage Buffer (Instance Index Table) with size: {} (bytes)", instanceIndexTable.byte_capacity());
    return std::make_shared<vk::resource::DynamicStorageBuffer>(std::move(instanceIndexTable));
}
[[nodiscard]] std::shared_ptr<vk::resource::DynamicStorageBuffer>
make_instance_info_table(const std::shared_ptr<vk::Allocator>& pAllocator, std::int32_t numFramesInFlight, std::uint32_t maxInstances)
{
    VkDeviceSize partitionSize = sizeof(InstanceInfo) * maxInstances;
    VkDeviceSize numPartitions = numFramesInFlight;
    bool transferDestination = true;
    bool indirectUsage = false;

    vk::resource::DynamicStorageBuffer instanceInfoTable =
        pAllocator->create_dynamic_storage_buffer(partitionSize, numPartitions, transferDestination, indirectUsage);
    LOG_INFO("Creating Dynamic Storage Buffer (Instance Info Table) with size: {} (bytes)", instanceInfoTable.byte_capacity());
    return std::make_shared<vk::resource::DynamicStorageBuffer>(std::move(instanceInfoTable));
}
[[nodiscard]] std::shared_ptr<vk::resource::DynamicUniformBuffer> make_camera_data(const std::shared_ptr<vk::Allocator>& pAllocator,
                                                                                   std::int32_t numFramesInFlight)
{
    VkDeviceSize partitionSize = sizeof(CameraInfo);
    VkDeviceSize numPartitions = numFramesInFlight;

    vk::resource::DynamicUniformBuffer cameraBuffer = pAllocator->create_dynamic_uniform_buffer(partitionSize, numPartitions);
    LOG_INFO("Creating Dynamic Uniform Buffer (Camera Buffer) with size: {} (bytes)", cameraBuffer.byte_capacity());
    return std::make_shared<vk::resource::DynamicUniformBuffer>(std::move(cameraBuffer));
}
}    // namespace
namespace odin::graphics::registry::resource::buffer
{
BufferRegistry::BufferRegistry(const std::shared_ptr<vk::Allocator>& pAllocator,
                               const FrameHandler& frameHandler,
                               std::int32_t maxDraws,
                               std::int32_t maxInstances)
    : m_pIndexBuffer{ make_index_buffer(pAllocator) }
    , m_pVertexBuffer{ make_vertex_buffer(pAllocator) }
    , m_DynSSBO{}
    , m_DynUBO{}
    , m_SSBO{}
{
    make_buffers(pAllocator, frameHandler.in_flight_count(), maxDraws, maxInstances);
}
std::reference_wrapper<const vk::resource::IndexBuffer> BufferRegistry::index_buffer() const
{
    ODIN_ASSERT(m_pIndexBuffer != nullptr);
    return *m_pIndexBuffer.get();
}
std::reference_wrapper<const vk::resource::VertexBuffer> BufferRegistry::vertex_buffer() const
{
    ODIN_ASSERT(m_pVertexBuffer != nullptr);
    return *m_pVertexBuffer.get();
}
BufferHandle<vk::resource::StorageBuffer> BufferRegistry::get_storage_buffer(std::string_view key) const
{
    std::string k{ key };
    ODIN_ASSERT(m_SSBO.contains(k));
    return { m_SSBO.at(k) };
}
DynamicBufferHandle<vk::resource::DynamicStorageBuffer> BufferRegistry::get_dynamic_storage_buffer(std::string_view key) const
{
    std::string k{ key };
    ODIN_ASSERT(m_DynSSBO.contains(k));
    return { m_DynSSBO.at(k) };
}
DynamicBufferHandle<vk::resource::DynamicUniformBuffer> BufferRegistry::get_dynamic_uniform_buffer(std::string_view key) const
{
    std::string k{ key };
    ODIN_ASSERT(m_DynUBO.contains(k));
    return { m_DynUBO.at(k) };
}
void BufferRegistry::make_buffers(const std::shared_ptr<vk::Allocator>& pAllocator,
                                  std::int32_t numFramesInFlight,
                                  std::int32_t maxDraws,
                                  std::int32_t maxInstances)
{
    m_SSBO.emplace(ResourceRegistry::SSBO_MESH_TABLE, make_mesh_table(pAllocator));
    m_SSBO.emplace(ResourceRegistry::SSBO_MATERIAL_TABLE, make_material_table(pAllocator));

    m_DynSSBO.emplace(ResourceRegistry::DYN_SSBO_DRAW_COUNT, make_draw_count_table(pAllocator, numFramesInFlight));
    m_DynSSBO.emplace(ResourceRegistry::DYN_SSBO_DRAW_ARGS, make_draw_args_table(pAllocator, numFramesInFlight, maxDraws));
    m_DynSSBO.emplace(ResourceRegistry::DYN_SSBO_INSTANCE_BASE, make_instance_base_table(pAllocator, numFramesInFlight, maxDraws));
    m_DynSSBO.emplace(ResourceRegistry::DYN_SSBO_INSTANCE_COUNTER, make_instance_counter_table(pAllocator, numFramesInFlight, maxDraws));
    m_DynSSBO.emplace(ResourceRegistry::DYN_SSBO_INSTANCE_INDEX, make_instance_index_table(pAllocator, numFramesInFlight, maxInstances));
    m_DynSSBO.emplace(ResourceRegistry::DYN_SSBO_INSTANCE_INFO, make_instance_info_table(pAllocator, numFramesInFlight, maxInstances));

    m_DynUBO.emplace(ResourceRegistry::DYN_UBO_CAMERA_DATA, make_camera_data(pAllocator, numFramesInFlight));
}
}    // namespace odin::graphics::registry::resource::buffer
