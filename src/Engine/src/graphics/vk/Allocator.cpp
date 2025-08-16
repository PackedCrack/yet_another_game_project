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
[[nodiscard]] constexpr VmaAllocationCreateInfo make_allocation_create_info()
{
    return VmaAllocationCreateInfo{
        .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
        .requiredFlags = VkMemoryPropertyFlagBits(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
        .priority = 1.0f,
    };
}
[[nodiscard]] constexpr VmaAllocationCreateInfo make_cpu_to_gpu_create_info()
{
    return VmaAllocationCreateInfo{
        .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
                 VMA_ALLOCATION_CREATE_MAPPED_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
        .requiredFlags = VkMemoryPropertyFlagBits(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
        .priority = 1.0f,
    };
}
[[nodiscard]] constexpr VmaAllocationCreateInfo make_gpu_only_create_info()
{
    return VmaAllocationCreateInfo{
        .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
        .requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        .priority = 1.0f,
    };
}
[[nodiscard]] VmaAllocationCreateInfo make_allocation_create_info(VmaMemoryUsage usage)
{
    if (usage == VMA_MEMORY_USAGE_GPU_ONLY)
    {
        return make_gpu_only_create_info();
    }
    else if (usage == VMA_MEMORY_USAGE_CPU_TO_GPU)
    {
        return make_gpu_only_create_info();
    }
    else if (usage == VMA_MEMORY_USAGE_CPU_ONLY)
    {
        return make_gpu_only_create_info();
    }

    std::unreachable();
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
        resource::AllocatedImage allocatedImage{};
        VmaAllocation allocation{};
        VK_CHECK(vmaCreateImage(m_Allocator,
                                std::addressof(imageInfo),
                                std::addressof(allocInfo),
                                std::addressof(allocatedImage.image),
                                std::addressof(allocation),
                                nullptr),
                 "Failed to allocate Vulkan Image.");

        allocatedImage.pAllocation = static_cast<void*>(allocation);
        return resource::Image{ std::move(pAllocator), allocatedImage, imageInfo.format };
    }
    void destroy_image(VkImage image, void* pAllocation) const
    {
        auto allocation = static_cast<VmaAllocation>(pAllocation);
        vmaDestroyImage(m_Allocator, image, allocation);
    }
private:
    VmaAllocator m_Allocator = nullptr;
};
Allocator::Allocator(const Instance& instance, const PhysicalDevice& gpu, const Device& device)
    : m_pImpl{ std::make_unique<Allocator::Impl>(instance, gpu, device) }
{}
[[nodiscard]] resource::Image Allocator::create_image_cpu_only(const VkImageCreateInfo& info)
{
    VmaAllocationCreateInfo allocInfo = make_allocation_create_info(VMA_MEMORY_USAGE_CPU_ONLY);
    return m_pImpl->create_image(shared_from_this(), allocInfo, info);
}
[[nodiscard]] resource::Image Allocator::create_image_gpu_only(const VkImageCreateInfo& info)
{
    VmaAllocationCreateInfo allocInfo = make_allocation_create_info(VMA_MEMORY_USAGE_GPU_ONLY);
    return m_pImpl->create_image(shared_from_this(), allocInfo, info);
}
[[nodiscard]] resource::Image Allocator::create_image_cpu_to_gpu(const VkImageCreateInfo& info)
{
    VmaAllocationCreateInfo allocInfo = make_allocation_create_info(VMA_MEMORY_USAGE_CPU_TO_GPU);
    return m_pImpl->create_image(shared_from_this(), allocInfo, info);
}
void Allocator::destroy_image(VkImage image, void* pAllocation) const
{
    m_pImpl->destroy_image(image, pAllocation);
}
}    // namespace odin::graphics::vk
