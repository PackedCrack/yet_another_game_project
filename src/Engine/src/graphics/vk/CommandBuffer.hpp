#pragma once

#include "CommandPool.hpp"
#include "Device.hpp"
//
//
namespace odin::graphics::vk
{
struct CommandBufferRef
{
	VkCommandBuffer handle;
};
class CommandBuffer
{
public:
	CommandBuffer(VkCommandBuffer cmdBuffer);
	~CommandBuffer() = default;
	CommandBuffer(const CommandBuffer& other) = delete;
	CommandBuffer(CommandBuffer&& other) noexcept;
	CommandBuffer& operator=(const CommandBuffer& other) = delete;
	CommandBuffer& operator=(CommandBuffer&& other) noexcept;
public:
	void reset();
	void begin();
	void end();
	[[nodiscard]] CommandBufferRef handle() const;
private:
	VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
};
}	// namespace odin::graphics::vk