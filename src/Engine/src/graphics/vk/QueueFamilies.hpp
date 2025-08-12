#pragma once

#include "PhysicalDevice.hpp"
#include "Surface.hpp"
// vulkan
#include <vulkan/vulkan.h>
//
//
namespace odin::graphics::vk
{
class QueueFamilies
{
public:
	using index_t = std::uint32_t;
	static constexpr index_t INVALID_INDEX = std::numeric_limits<index_t>::max();
	struct Queue
	{
		VkQueue handle;
		index_t index;
	};
	explicit QueueFamilies(const PhysicalDevice& device, const Surface& surface);
public:
	[[nodiscard]] VkQueue present() const;
	[[nodiscard]] VkQueue graphics() const;
	[[nodiscard]] VkQueue compute() const;
	[[nodiscard]] VkQueue transfer() const;
private:
	void select_queue_indices(const PhysicalDevice& device, const Surface& surface);
private:
	Queue m_Present;
	Queue m_Graphics;
	Queue m_Compute;
	Queue m_Transfer;
};
}	// namespace odin::graphics::vk