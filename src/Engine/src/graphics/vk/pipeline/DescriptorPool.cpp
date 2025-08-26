//
// Created by qwerty on 17/08/2025.
//
#include "DescriptorPool.hpp"

#include "../vulkan_defines.hpp"
//
//
namespace odin::graphics::vk::pipeline
{
DescriptorPool::DescriptorPool(DeviceRef device, bool updatedAfterBind)
    : m_DescriptorPool{ VK_NULL_HANDLE }
    , m_Device{ device }
{
    // Setting these sizes fairly uniform for now.. Can tweak them in the future based on needs
    static constexpr std::array<VkDescriptorPoolSize, 11u> poolSizes{
        VkDescriptorPoolSize{         VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        VkDescriptorPoolSize{   VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        VkDescriptorPoolSize{         VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        VkDescriptorPoolSize{   VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        VkDescriptorPoolSize{                VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        VkDescriptorPoolSize{          VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        VkDescriptorPoolSize{          VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        VkDescriptorPoolSize{       VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,  100 }
    };
    VkDescriptorPoolCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    info.flags = updatedAfterBind ? VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT : VK_NO_FLAGS;
    info.maxSets = 1024u;
    info.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    info.pPoolSizes = poolSizes.data();


    VK_CHECK(vkCreateDescriptorPool(m_Device.handle, std::addressof(info), nullptr, std::addressof(m_DescriptorPool)),
             "Failed to create Descriptor Pool.");
}
DescriptorPool::~DescriptorPool()
{
    if (m_DescriptorPool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(m_Device.handle, m_DescriptorPool, nullptr);
    }
}
DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept
    : m_DescriptorPool{ VK_NULL_HANDLE }
    , m_Device{ other.m_Device }
{
    std::swap(m_DescriptorPool, other.m_DescriptorPool);
}
DescriptorPool& DescriptorPool::operator=(DescriptorPool&& other) noexcept
{
    if (this != std::addressof(other))
    {
        std::swap(m_DescriptorPool, other.m_DescriptorPool);
        m_Device = other.m_Device;
    }

    return *this;
}
DescriptorPoolRef DescriptorPool::handle() const
{
    ODIN_ASSERT(m_DescriptorPool != VK_NULL_HANDLE);
    return DescriptorPoolRef{ .handle = m_DescriptorPool };
}
}    // namespace odin::graphics::vk::pipeline
