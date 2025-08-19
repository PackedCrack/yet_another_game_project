//
// Created by qwerty on 12/08/2025.
//
#include "Allocator.hpp"

#include "vulkan_defines.hpp"

#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 4296)     // expression is always false
    #pragma warning(disable: 4100)     // Unreferenced formal parameter
    #pragma warning(disable: 6387)     // could be 0
    #pragma warning(disable: 26451)    // arithmetic overflow
    #pragma warning(disable: 6386)     // Buffer overrun while writing.. lol wtf, amd?
    #pragma warning(disable: 26110)    // Caller failing to hold lock
    #pragma warning(disable: 4127)     // Conditional is constant
    #pragma warning(disable: 6011)     // Dereferencing null ptr
    #pragma warning(disable: 26813)    // Use bitwise and to check if a flag is set
    #pragma warning(disable: 26495)    // Variable is uninitialized
    #pragma warning(disable: 4324)     // structure was padded due to alighnment specifier
    #pragma warning(disable: 4189)     // local variable initialized but not referenced
    #pragma warning(disable: 4061)     // switch enum not explicitly handled
#else
    #error FIX WARNINGS CAUSED BY VMA FOR YOUR COMPILER
#endif

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#ifdef _MSC_VER
    #pragma warning(pop)
#else
    #error FIX WARNINGS CAUSED BY VMA FOR YOUR COMPILER
#endif
namespace
{
using namespace odin::graphics::vk;
//
//
[[nodiscard]] constexpr VmaAllocationCreateInfo host_writable_alloc_info()
{
    return VmaAllocationCreateInfo{ .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
                                    .usage = VMA_MEMORY_USAGE_AUTO,
                                    .priority = 1.0f };
}
[[nodiscard]] constexpr VmaAllocationCreateInfo device_only_alloc_info()
{
    return VmaAllocationCreateInfo{ .flags = VK_NO_FLAGS, .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, .priority = 1.0f };
}
[[nodiscard]] constexpr VmaAllocationCreateInfo dedicated_device_only_alloc_info()
{
    return VmaAllocationCreateInfo{ .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
                                    .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
                                    .priority = 1.0f };
}
[[nodiscard]] constexpr VmaAllocationCreateInfo uniform_buffer_alloc_info()
{
    return host_writable_alloc_info();
}
[[nodiscard]] constexpr VmaAllocationCreateInfo staging_buffer_alloc_info()
{
    return host_writable_alloc_info();
}
[[nodiscard]] constexpr VmaAllocationCreateInfo storage_buffer_alloc_info()
{
    return device_only_alloc_info();
}
[[nodiscard]] constexpr VmaAllocationCreateInfo dedicated_storage_buffer_alloc_info()
{
    return dedicated_device_only_alloc_info();
}
[[nodiscard]] constexpr VmaAllocationCreateInfo image_attachment_alloc_info()
{
    return dedicated_device_only_alloc_info();
}
[[nodiscard]] constexpr VmaAllocationCreateInfo image_alloc_info()
{
    return device_only_alloc_info();
}
[[nodiscard]] std::function<void(resource::AllocatedImage)> make_image_deleter(std::shared_ptr<Allocator> pAllocator)
{
    return [pAllocator = std::move(pAllocator)](resource::AllocatedImage image)
    {
        auto allocation = static_cast<VmaAllocation>(image.pAllocation);
        pAllocator->destroy_image(image.handle, allocation);
    };
}
[[nodiscard]] std::function<void(resource::AllocatedBuffer)> make_buffer_deleter(std::shared_ptr<Allocator> pAllocator)
{
    return [pAllocator = std::move(pAllocator)](resource::AllocatedBuffer buffer)
    {
        auto allocation = static_cast<VmaAllocation>(buffer.pAllocation);
        pAllocator->destroy_buffer(buffer.handle, allocation, buffer.pData);
    };
}
[[nodiscard]] VmaAllocator make_allocator(const odin::graphics::vk::Instance& instance,
                                          const odin::graphics::vk::PhysicalDevice& gpu,
                                          const odin::graphics::vk::Device& device)
{
    using namespace odin::graphics;

    VmaAllocatorCreateInfo info{};

    vk::InstanceRef inst = instance.handle();
    info.instance = inst.handle;
    vk::PhysicalDeviceRef g = gpu.handle();
    info.physicalDevice = g.handle;

    vk::DeviceRef dev = device.handle();
    info.device = dev.handle;

    VmaAllocator allocator{};
    VK_CHECK(vmaCreateAllocator(&info, std::addressof(allocator)), "Failed to create VMA Allocator.");

    return allocator;
}
}    // namespace
namespace odin::graphics::vk
{
class Allocator::Impl
{
public:
    Impl(const Instance& instance, const PhysicalDevice& gpu, const Device& device)
        : m_Allocator{ make_allocator(instance, gpu, device) }
    {}
    ~Impl()
    {
        if (m_Allocator != nullptr)
        {
            vmaDestroyAllocator(m_Allocator);
        }
    }
    Impl(const Impl& other) = delete;
    Impl(Impl&& other) noexcept
        : m_Allocator{ nullptr }
    {
        std::swap(m_Allocator, other.m_Allocator);
    }
    Impl& operator=(const Impl& other) = delete;
    Impl& operator=(Impl&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            m_Allocator = std::exchange(other.m_Allocator, m_Allocator);
        }
        return *this;
    }
public:
    resource::Image
    create_image(std::shared_ptr<Allocator> pAllocator, VmaAllocationCreateInfo allocInfo, const VkImageCreateInfo& imageInfo)
    {
        resource::AllocatedImage image{};
        VmaAllocation allocation{};
        VK_CHECK(vmaCreateImage(m_Allocator,
                                std::addressof(imageInfo),
                                std::addressof(allocInfo),
                                std::addressof(image.handle),
                                std::addressof(allocation),
                                nullptr),
                 "Failed to allocate Vulkan Image.");

        image.pAllocation = static_cast<void*>(allocation);
        return resource::Image{ image, make_image_deleter(std::move(pAllocator)), imageInfo.format };
    }
    resource::StagingBuffer create_staging_buffer(std::shared_ptr<Allocator> pAllocator, const VkBufferCreateInfo& info)
    {
        VmaAllocationCreateInfo allocInfo = staging_buffer_alloc_info();
        auto [handle, allocation] = create_buffer(info, allocInfo);

        resource::AllocatedBuffer buffer{ .handle = handle, .pAllocation = allocation, .pData = map_memory(allocation) };

        return resource::StagingBuffer{ buffer, make_buffer_deleter(std::move(pAllocator)) };
    }
    resource::UniformBuffer create_uniform_buffer(std::shared_ptr<Allocator> pAllocator, const VkBufferCreateInfo& info)
    {
        VmaAllocationCreateInfo allocInfo = uniform_buffer_alloc_info();
        auto [handle, allocation] = create_buffer(info, allocInfo);

        resource::AllocatedBuffer buffer{ .handle = handle, .pAllocation = allocation, .pData = map_memory(allocation) };

        return resource::UniformBuffer{ buffer, make_buffer_deleter(std::move(pAllocator)) };
    }
    resource::VertexBuffer
    create_vertex_buffer(std::shared_ptr<Allocator> pAllocator, std::uint32_t numElements, const VkBufferCreateInfo& info)
    {
        VmaAllocationCreateInfo allocInfo = storage_buffer_alloc_info();
        auto [handle, allocation] = create_buffer(info, allocInfo);

        resource::AllocatedBuffer buffer{ .handle = handle, .pAllocation = allocation, .pData = map_memory(allocation) };

        return resource::VertexBuffer{ numElements, buffer, make_buffer_deleter(std::move(pAllocator)) };
    }
    resource::StorageBuffer create_storage_buffer(std::shared_ptr<Allocator> pAllocator, const VkBufferCreateInfo& info)
    {
        VmaAllocationCreateInfo allocInfo = storage_buffer_alloc_info();
        auto [handle, allocation] = create_buffer(info, allocInfo);

        resource::AllocatedBuffer buffer{ .handle = handle, .pAllocation = allocation, .pData = map_memory(allocation) };

        return resource::StorageBuffer{ buffer, make_buffer_deleter(std::move(pAllocator)) };
    }
    void destroy_buffer(VkBuffer buffer, VmaAllocation allocation, const void* pData) const
    {
        if (pData != nullptr)
        {
            vmaUnmapMemory(m_Allocator, allocation);
        }
        vmaDestroyBuffer(m_Allocator, buffer, allocation);
    }
    void destroy_image(VkImage image, VmaAllocation allocation) const { vmaDestroyImage(m_Allocator, image, allocation); }
private:
    std::tuple<VkBuffer, VmaAllocation> create_buffer(const VkBufferCreateInfo& bufferInfo, const VmaAllocationCreateInfo& allocInfo) const
    {
        /*m_BufferSize = resource::pad_uniform_buffer_size(bufferSize, m_OffsetAlignment) * m_NumEntries;

        const VkBufferCreateInfo BUFFER_INFO{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = NULL,
            .size = m_BufferSize,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0u,
            .pQueueFamilyIndices = nullptr
        };*/

        VkBuffer buffer{};
        VmaAllocation pAllocation = nullptr;
        VK_CHECK(vmaCreateBuffer(m_Allocator,
                                 std::addressof(bufferInfo),
                                 std::addressof(allocInfo),
                                 std::addressof(buffer),
                                 std::addressof(pAllocation),
                                 nullptr),
                 "Failed to Create Buffer");

        return { buffer, pAllocation };
    }
    void* map_memory(VmaAllocation allocation)
    {
        void* pData = nullptr;
        VK_CHECK(vmaMapMemory(m_Allocator, allocation, std::addressof(pData)), "Failed to obtain pointer to mapped memory.");
        return pData;
    }
private:
    VmaAllocator m_Allocator = nullptr;
};
Allocator::Allocator(const Instance& instance, const PhysicalDevice& gpu, const Device& device)
    : m_pImpl{ std::make_unique<Allocator::Impl>(instance, gpu, device) }
{}
resource::StagingBuffer Allocator::create_staging_buffer(const VkBufferCreateInfo& info)
{
    return m_pImpl->create_staging_buffer(shared_from_this(), info);
}
resource::UniformBuffer Allocator::create_uniform_buffer(const VkBufferCreateInfo& info)
{
    return m_pImpl->create_uniform_buffer(shared_from_this(), info);
}
resource::StorageBuffer Allocator::create_storage_buffer(const VkBufferCreateInfo& info)
{
    return m_pImpl->create_storage_buffer(shared_from_this(), info);
}
resource::Image Allocator::create_image_attachment(const VkImageCreateInfo& info)
{
    VmaAllocationCreateInfo allocInfo = image_attachment_alloc_info();
    return m_pImpl->create_image(shared_from_this(), allocInfo, info);
}
resource::Image Allocator::create_image_texture(const VkImageCreateInfo& info)
{
    VmaAllocationCreateInfo allocInfo = image_alloc_info();
    return m_pImpl->create_image(shared_from_this(), allocInfo, info);
}
resource::VertexBuffer Allocator::create_vertex_buffer(std::uint32_t numElements, const VkBufferCreateInfo& info)
{
    return m_pImpl->create_vertex_buffer(shared_from_this(), numElements, info);
}
void Allocator::destroy_buffer(VkBuffer buffer, void* pAllocation, const void* pData) const
{
    auto allocation = static_cast<VmaAllocation>(pAllocation);
    m_pImpl->destroy_buffer(buffer, allocation, pData);
}
void Allocator::destroy_image(VkImage image, void* pAllocation) const
{
    auto allocation = static_cast<VmaAllocation>(pAllocation);
    m_pImpl->destroy_image(image, allocation);
}
}    // namespace odin::graphics::vk
