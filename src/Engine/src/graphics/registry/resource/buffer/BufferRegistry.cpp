//
// Created by qwerty on 07/09/2025.
//
#include "BufferRegistry.hpp"

#include "../ResourceRegistry.hpp"
#include "../../../vk/ext/instance/debug_utils.hpp"
#include "../../../gpu_types.hpp"
// debug
#include <debug/debug_defines.hpp>
#include <debug/Logger.hpp>
//
//
namespace
{
using namespace odin::graphics;
using namespace odin::graphics::vk::resource;
using namespace odin::graphics::registry::resource::buffer;
//
//
void assign_debug_name(vk::DeviceRef device, BufferRef buffer, std::string_view name)
{
    VkDebugUtilsObjectNameInfoEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    info.pNext = nullptr;
    info.objectType = VK_OBJECT_TYPE_BUFFER;
    info.objectHandle = reinterpret_cast<std::uint64_t>(buffer.handle);
    info.pObjectName = name.data();

    vk::ext::instance::vkSetDebugUtilsObjectName(device.handle, std::addressof(info));
}
[[nodiscard]] std::unique_ptr<IndexBuffer> make_index_buffer(vk::DeviceRef device, const std::shared_ptr<vk::Allocator>& pAllocator)
{
    static constexpr std::uint64_t indexCapacity = 512 * 128 * 128;    // Aproximately 8,3 million indices

    IndexBuffer indexBuffer = pAllocator->create_index_buffer(indexCapacity);
    LOG_INFO("Creating Index Buffer with size: {} (bytes)", indexBuffer.byte_capacity());

#ifdef BUFFER_NAMES
    assign_debug_name(device, indexBuffer.handle(), "index_buffer");
#endif

    return std::make_unique<IndexBuffer>(std::move(indexBuffer));
}
[[nodiscard]] std::unique_ptr<VertexBuffer> make_vertex_buffer(vk::DeviceRef device, const std::shared_ptr<vk::Allocator>& pAllocator)
{
    static constexpr std::uint64_t vertexCapacity = 512 * 128 * 128;    // Aproximately 8,3 million vertices

    VertexBuffer vertexBuffer = pAllocator->create_vertex_buffer(vertexCapacity);
    LOG_INFO("Creating Vertex Buffer with size: {} (bytes)", vertexBuffer.byte_capacity());

#ifdef BUFFER_NAMES
    assign_debug_name(device, vertexBuffer.handle(), "vertex_buffer");
#endif

    return std::make_unique<VertexBuffer>(std::move(vertexBuffer));
}
[[nodiscard]] std::shared_ptr<StorageBuffer> make_mesh_table(vk::DeviceRef device, const std::shared_ptr<vk::Allocator>& pAllocator)
{
    static constexpr std::uint64_t meshTableSize = 15000 * sizeof(odin::graphics::MeshInfo);

    StorageBuffer meshTable = pAllocator->create_storage_buffer(meshTableSize);
    LOG_INFO("Creating Storage Buffer (Mesh Table) with size: {} (bytes)", meshTable.byte_capacity());

#ifdef BUFFER_NAMES
    assign_debug_name(device, meshTable.handle(), "mesh_table");
#endif

    return std::make_shared<StorageBuffer>(std::move(meshTable));
}
[[nodiscard]] std::shared_ptr<StorageBuffer> make_material_table(vk::DeviceRef device, const std::shared_ptr<vk::Allocator>& pAllocator)
{
    static constexpr std::uint64_t materialTableSize = 1000 * sizeof(odin::graphics::Material);

    StorageBuffer materialTable = pAllocator->create_storage_buffer(materialTableSize);
    LOG_INFO("Creating Storage Buffer (Material Table) with size: {} (bytes)", materialTable.byte_capacity());

#ifdef BUFFER_NAMES
    assign_debug_name(device, materialTable.handle(), "material_table");
#endif

    return std::make_shared<StorageBuffer>(std::move(materialTable));
}
[[nodiscard]] std::shared_ptr<DynamicStorageBuffer>
make_draw_variables_table(vk::DeviceRef device, const std::shared_ptr<vk::Allocator>& pAllocator, std::int32_t numFramesInFlight)
{
    VkDeviceSize partitionSize = sizeof(DrawVariables);
    VkDeviceSize numPartitions = numFramesInFlight;
    bool transferDestination = true;
    bool indirectUsage = true;

    DynamicStorageBuffer drawVariablesTable =
        pAllocator->create_dynamic_storage_buffer(partitionSize, numPartitions, transferDestination, indirectUsage);
    LOG_INFO("Creating Dynamic Storage Buffer (Draw Variables Table) with size: {} (bytes)", drawVariablesTable.byte_capacity());

#ifdef BUFFER_NAMES
    assign_debug_name(device, drawVariablesTable.handle(), "draw_variables");
#endif

    return std::make_shared<DynamicStorageBuffer>(std::move(drawVariablesTable));
}
[[nodiscard]] std::shared_ptr<DynamicStorageBuffer> make_draw_args_table(vk::DeviceRef device,
                                                                         const std::shared_ptr<vk::Allocator>& pAllocator,
                                                                         std::int32_t numFramesInFlight,
                                                                         std::uint32_t maxDraws)
{
    VkDeviceSize partitionSize = sizeof(VkDrawIndexedIndirectCommand) * maxDraws;
    VkDeviceSize numPartitions = numFramesInFlight;
    bool transferDestination = false;
    bool indirectUsage = true;

    DynamicStorageBuffer drawArgsTable =
        pAllocator->create_dynamic_storage_buffer(partitionSize, numPartitions, transferDestination, indirectUsage);
    LOG_INFO("Creating Dynamic Storage Buffer (Draw Arguments Table) with size: {} (bytes)", drawArgsTable.byte_capacity());

#ifdef BUFFER_NAMES
    assign_debug_name(device, drawArgsTable.handle(), "draw_args");
#endif

    return std::make_shared<DynamicStorageBuffer>(std::move(drawArgsTable));
}
[[nodiscard]] std::shared_ptr<DynamicStorageBuffer> make_instance_base_table(vk::DeviceRef device,
                                                                             const std::shared_ptr<vk::Allocator>& pAllocator,
                                                                             std::int32_t numFramesInFlight,
                                                                             std::uint32_t maxDraws)
{
    VkDeviceSize partitionSize = sizeof(std::uint32_t) * maxDraws;
    VkDeviceSize numPartitions = numFramesInFlight;
    bool transferDestination = false;
    bool indirectUsage = false;

    DynamicStorageBuffer instanceBaseTable =
        pAllocator->create_dynamic_storage_buffer(partitionSize, numPartitions, transferDestination, indirectUsage);
    LOG_INFO("Creating Dynamic Storage Buffer (Instance Base Table) with size: {} (bytes)", instanceBaseTable.byte_capacity());

#ifdef BUFFER_NAMES
    assign_debug_name(device, instanceBaseTable.handle(), "instance_base");
#endif

    return std::make_shared<DynamicStorageBuffer>(std::move(instanceBaseTable));
}
[[nodiscard]] std::shared_ptr<DynamicStorageBuffer> make_instance_counter_table(vk::DeviceRef device,
                                                                                const std::shared_ptr<vk::Allocator>& pAllocator,
                                                                                std::int32_t numFramesInFlight,
                                                                                std::uint32_t maxDraws)
{
    VkDeviceSize partitionSize = sizeof(std::uint32_t) * maxDraws;
    VkDeviceSize numPartitions = numFramesInFlight;
    bool transferDestination = true;    // This has to be transfer dst because the FrustumCull pass zeroes it with vkCmdFill
    bool indirectUsage = false;

    DynamicStorageBuffer instanceCounterTable =
        pAllocator->create_dynamic_storage_buffer(partitionSize, numPartitions, transferDestination, indirectUsage);
    LOG_INFO("Creating Dynamic Storage Buffer (Instance Counter Table) with size: {} (bytes)", instanceCounterTable.byte_capacity());

#ifdef BUFFER_NAMES
    assign_debug_name(device, instanceCounterTable.handle(), "instance_counter");
#endif

    return std::make_shared<DynamicStorageBuffer>(std::move(instanceCounterTable));
}
[[nodiscard]] std::shared_ptr<DynamicStorageBuffer> make_instance_index_table(vk::DeviceRef device,
                                                                              const std::shared_ptr<vk::Allocator>& pAllocator,
                                                                              std::int32_t numFramesInFlight,
                                                                              std::uint32_t maxInstances)
{
    VkDeviceSize partitionSize = sizeof(std::uint32_t) * maxInstances;
    VkDeviceSize numPartitions = numFramesInFlight;
    bool transferDestination = false;
    bool indirectUsage = false;

    DynamicStorageBuffer instanceIndexTable =
        pAllocator->create_dynamic_storage_buffer(partitionSize, numPartitions, transferDestination, indirectUsage);
    LOG_INFO("Creating Dynamic Storage Buffer (Instance Index Table) with size: {} (bytes)", instanceIndexTable.byte_capacity());

#ifdef BUFFER_NAMES
    assign_debug_name(device, instanceIndexTable.handle(), "instance_index");
#endif

    return std::make_shared<DynamicStorageBuffer>(std::move(instanceIndexTable));
}
[[nodiscard]] std::shared_ptr<DynamicStorageBuffer> make_instance_info_table(vk::DeviceRef device,
                                                                             const std::shared_ptr<vk::Allocator>& pAllocator,
                                                                             std::int32_t numFramesInFlight,
                                                                             std::uint32_t maxInstances)
{
    VkDeviceSize partitionSize = sizeof(InstanceInfo) * maxInstances;
    VkDeviceSize numPartitions = numFramesInFlight;
    bool transferDestination = true;
    bool indirectUsage = false;

    DynamicStorageBuffer instanceInfoTable =
        pAllocator->create_dynamic_storage_buffer(partitionSize, numPartitions, transferDestination, indirectUsage);
    LOG_INFO("Creating Dynamic Storage Buffer (Instance Info Table) with size: {} (bytes)", instanceInfoTable.byte_capacity());

#ifdef BUFFER_NAMES
    assign_debug_name(device, instanceInfoTable.handle(), "instance_info");
#endif

    return std::make_shared<DynamicStorageBuffer>(std::move(instanceInfoTable));
}
[[nodiscard]] std::shared_ptr<DynamicUniformBuffer>
make_camera_data(vk::DeviceRef device, const std::shared_ptr<vk::Allocator>& pAllocator, std::int32_t numFramesInFlight)
{
    VkDeviceSize partitionSize = sizeof(CameraInfo);
    VkDeviceSize numPartitions = numFramesInFlight;

    DynamicUniformBuffer cameraBuffer = pAllocator->create_dynamic_uniform_buffer(partitionSize, numPartitions);
    LOG_INFO("Creating Dynamic Uniform Buffer (Camera Buffer) with size: {} (bytes)", cameraBuffer.byte_capacity());

#ifdef BUFFER_NAMES
    assign_debug_name(device, cameraBuffer.handle(), "camera_buffer");
#endif

    return std::make_shared<DynamicUniformBuffer>(std::move(cameraBuffer));
}
}    // namespace
namespace odin::graphics::registry::resource::buffer
{
BufferRegistry::BufferRegistry(vk::DeviceRef device,
                               const std::shared_ptr<vk::Allocator>& pAllocator,
                               const FrameHandler& frameHandler,
                               std::int32_t maxDraws,
                               std::int32_t maxInstances)
    : m_pIndexBuffer{ make_index_buffer(device, pAllocator) }
    , m_pVertexBuffer{ make_vertex_buffer(device, pAllocator) }
    , m_DynSSBO{}
    , m_DynUBO{}
    , m_SSBO{}
{
    make_buffers(device, pAllocator, frameHandler.in_flight_count(), maxDraws, maxInstances);
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
void BufferRegistry::make_buffers(vk::DeviceRef device,
                                  const std::shared_ptr<vk::Allocator>& pAllocator,
                                  std::int32_t numFramesInFlight,
                                  std::int32_t maxDraws,
                                  std::int32_t maxInstances)
{
    m_SSBO.emplace(ResourceRegistry::SSBO_MESH_TABLE, make_mesh_table(device, pAllocator));
    m_SSBO.emplace(ResourceRegistry::SSBO_MATERIAL_TABLE, make_material_table(device, pAllocator));

    m_DynSSBO.emplace(ResourceRegistry::DYN_SSBO_DRAW_VARIABLES, make_draw_variables_table(device, pAllocator, numFramesInFlight));
    m_DynSSBO.emplace(ResourceRegistry::DYN_SSBO_DRAW_ARGS, make_draw_args_table(device, pAllocator, numFramesInFlight, maxDraws));
    m_DynSSBO.emplace(ResourceRegistry::DYN_SSBO_INSTANCE_BASE, make_instance_base_table(device, pAllocator, numFramesInFlight, maxDraws));
    m_DynSSBO.emplace(ResourceRegistry::DYN_SSBO_INSTANCE_COUNTER,
                      make_instance_counter_table(device, pAllocator, numFramesInFlight, maxDraws));
    m_DynSSBO.emplace(ResourceRegistry::DYN_SSBO_INSTANCE_INDEX,
                      make_instance_index_table(device, pAllocator, numFramesInFlight, maxInstances));
    m_DynSSBO.emplace(ResourceRegistry::DYN_SSBO_INSTANCE_INFO,
                      make_instance_info_table(device, pAllocator, numFramesInFlight, maxInstances));

    m_DynUBO.emplace(ResourceRegistry::DYN_UBO_CAMERA_DATA, make_camera_data(device, pAllocator, numFramesInFlight));
}
}    // namespace odin::graphics::registry::resource::buffer
