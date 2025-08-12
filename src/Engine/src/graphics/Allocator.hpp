#pragma once

#include "vk/Device.hpp"
#include "vk/Image.hpp"
#include "vk/Instance.hpp"
#include "vk/PhysicalDevice.hpp"

//
//
namespace odin::graphics
{
struct AllocatedImage
{
	VkImage image = VK_NULL_HANDLE;
	void* pAllocation = nullptr;	// This is pointer to VmaAllocation
};
class Allocator : std::enable_shared_from_this<Allocator>
{
	class Impl;
public:
	Allocator(const vk::Instance& instance, const vk::PhysicalDevice& gpu, const vk::Device& device);
public:
	[[nodiscard]] vk::Image create_image_cpu_only(const VkImageCreateInfo& info);
	[[nodiscard]] vk::Image create_image_gpu_only(const VkImageCreateInfo& info);
	[[nodiscard]] vk::Image create_image_cpu_to_gpu(const VkImageCreateInfo& info);
	void destroy_image(VkImage image, void* pAllocation) const;
private:
	std::unique_ptr<Impl> m_pImpl;
};
}	// namespace odin::graphics