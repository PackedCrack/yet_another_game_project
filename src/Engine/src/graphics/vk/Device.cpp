#include "Device.hpp"

#include "QueueFamilies.hpp"
#include "vulkan_defines.hpp"
#include "vulkan_info.hpp"
//
//
namespace
{
[[nodiscard]] VkPhysicalDeviceVulkan14Features required_v14_features()
{
    VkPhysicalDeviceVulkan14Features v14features{};
    v14features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES;
    v14features.pNext = nullptr;
    v14features.dynamicRenderingLocalRead = VK_TRUE;

    return v14features;
}
[[nodiscard]] VkPhysicalDeviceVulkan13Features required_v13_features(VkPhysicalDeviceVulkan14Features* p14Features)
{
    VkPhysicalDeviceVulkan13Features v13features{};
    v13features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    v13features.pNext = p14Features;
    v13features.synchronization2 = VK_TRUE;
    v13features.dynamicRendering = VK_TRUE;

    return v13features;
}
[[nodiscard]] VkPhysicalDeviceVulkan12Features required_v12_features(VkPhysicalDeviceVulkan13Features* p13Features)
{
    VkPhysicalDeviceVulkan12Features v12features{};
    v12features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    v12features.pNext = p13Features;
    v12features.drawIndirectCount = VK_TRUE;
    v12features.descriptorIndexing = VK_TRUE;
    v12features.descriptorBindingPartiallyBound = VK_TRUE;
    v12features.runtimeDescriptorArray = VK_TRUE;

    return v12features;
}
[[nodiscard]] VkPhysicalDeviceVulkan11Features required_v11_features(VkPhysicalDeviceVulkan12Features* p12Features)
{
    VkPhysicalDeviceVulkan11Features v11features{};
    v11features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    v11features.pNext = p12Features;
    v11features.shaderDrawParameters = VK_TRUE;

    return v11features;
}
[[nodiscard]] VkPhysicalDeviceFeatures required_v10_features()
{
    VkPhysicalDeviceFeatures v10features{};
    v10features.pipelineStatisticsQuery = VK_TRUE;

    return v10features;
}
[[nodiscard]] VkPhysicalDeviceFeatures2 required_features(VkPhysicalDeviceVulkan11Features* p11Features = nullptr)
{
    return VkPhysicalDeviceFeatures2{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
                                      .pNext = p11Features,
                                      .features = required_v10_features() };
}
}    // namespace
namespace odin::graphics::vk
{
Device::Device(const PhysicalDevice& phyDevice, QueueFamilies& queueFamilies)
    : m_Device{ VK_NULL_HANDLE }
{
    static constexpr std::array<const char*, 4> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                                                     VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME,
                                                                     VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME,
                                                                     VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME };

    VkPhysicalDeviceVulkan14Features v14features = required_v14_features();
    VkPhysicalDeviceVulkan13Features v13features = required_v13_features(std::addressof(v14features));
    VkPhysicalDeviceVulkan12Features v12features = required_v12_features(std::addressof(v13features));
    VkPhysicalDeviceVulkan11Features v11features = required_v11_features(std::addressof(v12features));
    VkPhysicalDeviceFeatures2 features = required_features(std::addressof(v11features));


    PhysicalDeviceRef phyDev = phyDevice.handle();
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfo = queueFamilies.queue_create_info();
    VkDeviceCreateInfo info = device_create_info(queueCreateInfo, deviceExtensions, features);
    VK_CHECK(vkCreateDevice(phyDev.handle, &info, nullptr, &m_Device), "Failed to create Vulkan Device.");

    queueFamilies.store_queue_handles(*this);
}
Device::~Device()
{
    if (m_Device != VK_NULL_HANDLE)
    {
        vkDestroyDevice(m_Device, nullptr);
    }
}
Device::Device(Device&& other) noexcept
    : m_Device{ std::exchange(other.m_Device, m_Device) }
{}
Device& Device::operator=(Device&& other) noexcept
{
    if (this != std::addressof(other))
    {
        m_Device = std::exchange(other.m_Device, m_Device);
    }

    return *this;
}
DeviceRef Device::handle() const
{
    ODIN_ASSERT(m_Device != VK_NULL_HANDLE);

    return DeviceRef{ .handle = m_Device };
}
[[nodiscard]] VkQueue Device::get_queue_handle(std::uint32_t familyIndex) const
{
    VkQueue handle = VK_NULL_HANDLE;
    vkGetDeviceQueue(m_Device, familyIndex, 0, std::addressof(handle));

    return handle;
}
}    // namespace odin::graphics::vk
