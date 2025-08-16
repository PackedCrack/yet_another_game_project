#include "CommandBuffer.hpp"

#include "vulkan_defines.hpp"
//
//
namespace odin::graphics::vk
{
CommandBuffer::CommandBuffer(VkCommandBuffer cmdBuffer)
	: m_CommandBuffer{ cmdBuffer }
{
	ODIN_ASSERT(cmdBuffer != VK_NULL_HANDLE);
}
CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept
	: m_CommandBuffer{ VK_NULL_HANDLE }
{
	std::swap(m_CommandBuffer, other.m_CommandBuffer);
}
CommandBuffer& CommandBuffer::operator=(CommandBuffer&& other) noexcept
{
	if (this != std::addressof(other))
	{
		m_CommandBuffer = std::exchange(other.m_CommandBuffer, m_CommandBuffer);
	}

	return *this;
}
void CommandBuffer::reset()
{
	VK_CHECK(vkResetCommandBuffer(m_CommandBuffer, VK_NO_FLAGS), "Failed to reset Vulkan Command Buffer.");
}
void CommandBuffer::begin()
{
	static constexpr VkCommandBufferBeginInfo info =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		.pInheritanceInfo = nullptr
	};
	VK_CHECK(vkBeginCommandBuffer(m_CommandBuffer, std::addressof(info)), "Failed to Begin Command Buffer.");
}
void CommandBuffer::end()
{
	VK_CHECK(vkEndCommandBuffer(m_CommandBuffer), "Failed to End Command Buffer.");
}
CommandBufferRef CommandBuffer::handle() const
{
	ODIN_ASSERT(m_CommandBuffer != VK_NULL_HANDLE);
	return CommandBufferRef{ .handle = m_CommandBuffer };
}
}	// namespace odin::graphics::vk