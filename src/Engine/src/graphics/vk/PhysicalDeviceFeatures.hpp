#pragma once

#include <vulkan/vulkan.h>

//
//
namespace odin::graphics::vk
{
class PhysicalDeviceFeatures
{
public:
	PhysicalDeviceFeatures() = default;
	PhysicalDeviceFeatures(VkPhysicalDevice gpu);
	~PhysicalDeviceFeatures() = default;
	PhysicalDeviceFeatures(const PhysicalDeviceFeatures& other);
	PhysicalDeviceFeatures(PhysicalDeviceFeatures&& other) noexcept;
	PhysicalDeviceFeatures& operator=(const PhysicalDeviceFeatures& other);
	PhysicalDeviceFeatures& operator=(PhysicalDeviceFeatures&& other) noexcept;
public:
	[[nodiscard]] VkBool32 descriptor_binding_partially_bound() const;
	[[nodiscard]] VkBool32 descriptor_indexing() const;
	[[nodiscard]] VkBool32 draw_indirect_count() const;
	[[nodiscard]] VkBool32 dynamic_rendering() const;
	[[nodiscard]] VkBool32 dynamic_rendering_local_read() const;
	[[nodiscard]] VkBool32 multi_draw_indirect() const;
	[[nodiscard]] VkBool32 pipeline_statistics_query() const;
	[[nodiscard]] VkBool32 runtime_descriptor_array() const;
	[[nodiscard]] VkBool32 shader_draw_parameters() const;
	[[nodiscard]] VkBool32 synchronization2() const;
	[[nodiscard]] bool supports_dynamic_state2() const;
	[[nodiscard]] bool supports_dynamic_state3() const;
private:
	void update_linked_list();
private:
	std::optional<VkPhysicalDeviceExtendedDynamicState3FeaturesEXT> m_DynamicState3Features;
	std::optional<VkPhysicalDeviceExtendedDynamicState2FeaturesEXT> m_DynamicState2Features;
	VkPhysicalDeviceVulkan14Features m_v14Features;
	VkPhysicalDeviceVulkan13Features m_v13Features;
	VkPhysicalDeviceVulkan12Features m_v12Features;
	VkPhysicalDeviceVulkan11Features m_v11Features;
	VkPhysicalDeviceFeatures2 m_Features;
};
}	// namespace odin::graphics::vk