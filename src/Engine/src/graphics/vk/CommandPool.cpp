#include "CommandPool.hpp"

#include "vulkan_defines.hpp"
#include "vulkan_info.hpp"
//
//
namespace odin::graphics::vk
{
CommandPool::CommandPool(DeviceRef device, QueueView queueFamily)
	: m_CommandPool{ VK_NULL_HANDLE }
	, m_Device{ device }
{
	ODIN_ASSERT(m_Device.handle != VK_NULL_HANDLE);

	VkCommandPoolCreateInfo info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = queueFamily.index
	};
	VK_CHECK(vkCreateCommandPool(m_Device.handle, std::addressof(info), nullptr, std::addressof(m_CommandPool)), 
			 "Failed to create Command Pool for queue: {}", queueFamily.index);
}
CommandPool::~CommandPool()
{
	if (m_CommandPool != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(m_Device.handle, m_CommandPool, nullptr);
	}
}
CommandPool::CommandPool(CommandPool&& other) noexcept
	: m_CommandPool{ VK_NULL_HANDLE }
	, m_Device{ other.m_Device }
{
	std::swap(m_CommandPool, other.m_CommandPool);
}
CommandPool& CommandPool::operator=(CommandPool&& other) noexcept
{
	if (this != std::addressof(other))
	{
		m_CommandPool = std::exchange(other.m_CommandPool, m_CommandPool);
		m_Device = std::exchange(other.m_Device, m_Device);
	}

	return *this;
}
CommandPoolRef CommandPool::handle() const
{
	ODIN_ASSERT(m_CommandPool != VK_NULL_HANDLE);
	return CommandPoolRef{ .handle = m_CommandPool };
}
CommandBuffer CommandPool::make_command_buffer() const
{
	VkCommandBufferAllocateInfo info = command_buffer_allocate_info(m_CommandPool);
	VkCommandBuffer cmdBuffer{};
	VK_CHECK(vkAllocateCommandBuffers(m_Device.handle, std::addressof(info), std::addressof(cmdBuffer)),
			 "Failed to allocate Vulkan Command Buffer.");
	return CommandBuffer{ cmdBuffer };
}
}	// namespace odin::graphics::vk