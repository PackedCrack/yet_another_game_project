//
// Created by qwerty on 11/08/2025.
//
#include "PhysicalDeviceFeatures.hpp"

#include "vulkan_defines.hpp"
//
//
namespace
{
[[nodiscard]] constexpr VkPhysicalDeviceExtendedDynamicState2FeaturesEXT
physical_device_features_dynamic_state2(VkPhysicalDeviceExtendedDynamicState3FeaturesEXT* pDynamicState3 = nullptr)
{
    return VkPhysicalDeviceExtendedDynamicState2FeaturesEXT{ .sType =
                                                                 VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT,
                                                             .pNext = pDynamicState3 };
}
[[nodiscard]] constexpr VkPhysicalDeviceExtendedDynamicState3FeaturesEXT physical_device_features_dynamic_state3()
{
    return VkPhysicalDeviceExtendedDynamicState3FeaturesEXT{ .sType =
                                                                 VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_FEATURES_EXT,
                                                             .pNext = nullptr };
}
[[nodiscard]] constexpr VkPhysicalDeviceFeatures2 physical_device_features_2(VkPhysicalDeviceVulkan11Features* pFeatures11 = nullptr)
{
    return VkPhysicalDeviceFeatures2{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = pFeatures11 };
}
[[nodiscard]] constexpr VkPhysicalDeviceVulkan11Features
physical_device_features_vulkan_11(VkPhysicalDeviceVulkan12Features* pFeatures12 = nullptr)
{
    return VkPhysicalDeviceVulkan11Features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, .pNext = pFeatures12 };
}
[[nodiscard]] constexpr VkPhysicalDeviceVulkan12Features physical_device_features_vulkan_12(VkPhysicalDeviceVulkan13Features* pFeatures13)
{
    return VkPhysicalDeviceVulkan12Features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, .pNext = pFeatures13 };
}
[[nodiscard]] constexpr VkPhysicalDeviceVulkan13Features physical_device_features_vulkan_13(VkPhysicalDeviceVulkan14Features* pFeatures14)
{
    return VkPhysicalDeviceVulkan13Features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, .pNext = pFeatures14 };
}
[[nodiscard]] constexpr VkPhysicalDeviceVulkan14Features physical_device_features_vulkan_14(void* pNext = nullptr)
{
    return VkPhysicalDeviceVulkan14Features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES, .pNext = pNext };
}
[[nodiscard]] std::vector<VkExtensionProperties> device_extension_properties(VkPhysicalDevice gpu)
{
    std::uint32_t count{};
    VK_RESULT(vkEnumerateDeviceExtensionProperties(gpu, nullptr, std::addressof(count), nullptr), "Failed to enumerate GPU Extensions.");
    std::vector<VkExtensionProperties> extensions{};
    extensions.resize(count);
    VK_RESULT(vkEnumerateDeviceExtensionProperties(gpu, nullptr, std::addressof(count), extensions.data()),
              "Failed to enumerate GPU Extensions.");

    return extensions;
}
[[nodiscard]] bool supports_extended_dynamic_state(VkPhysicalDevice gpu)
{
    std::vector<VkExtensionProperties> extensions = device_extension_properties(gpu);
    std::array<std::string_view, 2> extNames{ "VK_EXT_extended_dynamic_state2", "VK_EXT_extended_dynamic_state3" };

    bool state1{};
    bool state2{};
    for (auto&& ext : extensions)
    {
        if (ext.extensionName == extNames.front())
        {
            state1 = true;
        }
        if (ext.extensionName == extNames.back())
        {
            state2 = true;
        }
        if (state1 && state2)
        {
            break;
        }
    }

    return state1 && state2;
}
}    // namespace
namespace odin::graphics::vk
{
PhysicalDeviceFeatures::PhysicalDeviceFeatures(VkPhysicalDevice gpu)
    : m_DynamicState3Features{ std::nullopt }
    , m_DynamicState2Features{ std::nullopt }
    , m_v14Features{ physical_device_features_vulkan_14() }
    , m_v13Features{ physical_device_features_vulkan_13(std::addressof(m_v14Features)) }
    , m_v12Features{ physical_device_features_vulkan_12(std::addressof(m_v13Features)) }
    , m_v11Features{ physical_device_features_vulkan_11(std::addressof(m_v12Features)) }
    , m_Features{ physical_device_features_2(std::addressof(m_v11Features)) }
{
    ODIN_ASSERT(gpu != VK_NULL_HANDLE);

    if (supports_extended_dynamic_state(gpu))
    {
        m_DynamicState3Features = std::make_optional(physical_device_features_dynamic_state3());

        VkPhysicalDeviceExtendedDynamicState3FeaturesEXT& state3 = m_DynamicState3Features.value();
        m_DynamicState2Features = std::make_optional(physical_device_features_dynamic_state2(std::addressof(state3)));

        VkPhysicalDeviceExtendedDynamicState2FeaturesEXT& state2 = m_DynamicState2Features.value();
        m_v14Features.pNext = std::addressof(state2);
    }

    vkGetPhysicalDeviceFeatures2(gpu, std::addressof(m_Features));
}
PhysicalDeviceFeatures::PhysicalDeviceFeatures(const PhysicalDeviceFeatures& other)
    : m_DynamicState3Features{ other.m_DynamicState3Features }
    , m_DynamicState2Features{ other.m_DynamicState2Features }
    , m_v14Features{ other.m_v14Features }
    , m_v13Features{ other.m_v13Features }
    , m_v12Features{ other.m_v12Features }
    , m_v11Features{ other.m_v11Features }
    , m_Features{ other.m_Features }
{
    update_linked_list();
}
PhysicalDeviceFeatures::PhysicalDeviceFeatures(PhysicalDeviceFeatures&& other) noexcept
    : m_DynamicState3Features{ std::move(other.m_DynamicState3Features) }
    , m_DynamicState2Features{ std::move(other.m_DynamicState2Features) }
    , m_v14Features{ std::move(other.m_v14Features) }
    , m_v13Features{ std::move(other.m_v13Features) }
    , m_v12Features{ std::move(other.m_v12Features) }
    , m_v11Features{ std::move(other.m_v11Features) }
    , m_Features{ std::move(other.m_Features) }
{
    update_linked_list();
}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::operator=(const PhysicalDeviceFeatures& other)
{
    if (this != std::addressof(other))
    {
        m_Features = other.m_Features;
        m_v11Features = other.m_v11Features;
        m_v12Features = other.m_v12Features;
        m_v13Features = other.m_v13Features;
        m_v14Features = other.m_v14Features;

        update_linked_list();
    }
    return *this;
}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::operator=(PhysicalDeviceFeatures&& other) noexcept
{
    if (this != std::addressof(other))
    {
        m_Features = std::move(other.m_Features);
        m_v11Features = std::move(other.m_v11Features);
        m_v12Features = std::move(other.m_v12Features);
        m_v13Features = std::move(other.m_v13Features);
        m_v14Features = std::move(other.m_v14Features);

        update_linked_list();
    }
    return *this;
}
VkBool32 PhysicalDeviceFeatures::timeline_semaphore() const
{
    return m_v12Features.timelineSemaphore;
}
VkBool32 PhysicalDeviceFeatures::descriptor_binding_partially_bound() const
{
    return m_v12Features.descriptorBindingPartiallyBound;
}
VkBool32 PhysicalDeviceFeatures::descriptor_update_after_bind() const
{
    return m_v12Features.descriptorBindingStorageBufferUpdateAfterBind;
}
VkBool32 PhysicalDeviceFeatures::descriptor_indexing() const
{
    return m_v12Features.descriptorIndexing;
}
VkBool32 PhysicalDeviceFeatures::draw_indirect_count() const
{
    return m_v12Features.drawIndirectCount;
}
VkBool32 PhysicalDeviceFeatures::dynamic_rendering() const
{
    return m_v13Features.dynamicRendering;
}
VkBool32 PhysicalDeviceFeatures::dynamic_rendering_local_read() const
{
    return m_v14Features.dynamicRenderingLocalRead;
}
VkBool32 PhysicalDeviceFeatures::multi_draw_indirect() const
{
    return m_Features.features.multiDrawIndirect;
}
VkBool32 PhysicalDeviceFeatures::pipeline_statistics_query() const
{
    return m_Features.features.pipelineStatisticsQuery;
}
VkBool32 PhysicalDeviceFeatures::runtime_descriptor_array() const
{
    return m_v12Features.runtimeDescriptorArray;
}
VkBool32 PhysicalDeviceFeatures::shader_draw_parameters() const
{
    return m_v11Features.shaderDrawParameters;
}
VkBool32 PhysicalDeviceFeatures::synchronization2() const
{
    return m_v13Features.synchronization2;
}
bool PhysicalDeviceFeatures::supports_dynamic_state2() const
{
    return m_DynamicState2Features.has_value();
}
bool PhysicalDeviceFeatures::supports_dynamic_state3() const
{
    return m_DynamicState3Features.has_value();
}
void PhysicalDeviceFeatures::update_linked_list()
{
    m_Features.pNext = std::addressof(m_v11Features);
    m_v11Features.pNext = std::addressof(m_v12Features);
    m_v12Features.pNext = std::addressof(m_v13Features);
    m_v13Features.pNext = std::addressof(m_v14Features);
    if (m_DynamicState2Features)
    {
        m_v14Features.pNext = std::addressof(m_DynamicState2Features.value());
        m_DynamicState2Features->pNext = std::addressof(m_DynamicState3Features.value());
    }
}
}    // namespace odin::graphics::vk
