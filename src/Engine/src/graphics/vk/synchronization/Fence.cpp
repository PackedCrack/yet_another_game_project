#include "Fence.hpp"

#include "../vulkan_defines.hpp"
#include "../vulkan_info.hpp"
//
//
namespace odin::graphics::vk::synchronization
{
Fence::Fence(DeviceRef device, bool startSignaled)
	: m_Fence{ VK_NULL_HANDLE }
	, m_Device{ device }
{
	VkFenceCreateInfo info = fence_create_info(startSignaled);
	VK_CHECK(vkCreateFence(m_Device.handle, std::addressof(info), nullptr, std::addressof(m_Fence)), "Failed to create Vulkan Fence.");
}
Fence::~Fence()
{
	if (m_Fence != VK_NULL_HANDLE)
	{
		vkDestroyFence(m_Device.handle, m_Fence, nullptr);
	}
}
Fence::Fence(Fence&& other) noexcept
	: m_Fence{ std::exchange(other.m_Fence, m_Fence) }
	, m_Device{ std::exchange(other.m_Device, m_Device) }
{}
Fence& Fence::operator=(Fence&& other) noexcept
{
	if (this != std::addressof(other))
	{
		m_Fence = std::exchange(other.m_Fence, m_Fence);
		m_Device = std::exchange(other.m_Device, m_Device);
	}

	return *this;
}
FenceRef Fence::handle() const
{
	ODIN_ASSERT(m_Fence != VK_NULL_HANDLE);
	return FenceRef{ .handle = m_Fence };
}
}	// namespace odin::graphics::vk::synchronization