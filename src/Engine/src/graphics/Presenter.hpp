#pragma once

#include "vk/Device.hpp"
#include "vk/PhysicalDevice.hpp"
#include "vk/Swapchain.hpp"
#include "vk/Surface.hpp"
//
//
namespace odin::graphics
{
class Presenter
{
public:
	Presenter(const vk::Device& device, const vk::PhysicalDevice& physDevice, vk::Surface surface);
public:
	void present(const vk::QueueView& present, const std::vector<VkSemaphore>& rendering);
private:
	void rebuild_and_present(const vk::QueueView& present, const std::vector<VkSemaphore>& rendering, VkSwapchainKHR oldSwapchain);
private:
	vk::DeviceRef m_Device;
	vk::PhysicalDeviceRef m_PhysDevice;
	vk::Surface m_Surface;
	vk::Swapchain m_Swapchain;
};
}	// namespace odin::graphics