#include "Presenter.hpp"

#include "vk/vulkan_defines.hpp"
//
//
namespace odin::graphics
{
Presenter::Presenter(const vk::Device& device, const vk::PhysicalDevice& physDevice, vk::Surface surface)
	: m_Device{ device.handle() }
	, m_PhysDevice{ physDevice.handle() }
	, m_Surface{ std::move(surface) }
	, m_Swapchain{ device, physDevice, m_Surface }
{}
void Presenter::present(const vk::QueueView& present, const std::vector<VkSemaphore>& rendering)
{
    vk::SwapchainRef swapchain = m_Swapchain.handle();
    VkPresentInfoKHR info = { .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                              .pNext = nullptr,
                              .waitSemaphoreCount = static_cast<uint32_t>(rendering.size()),
                              .pWaitSemaphores = rendering.empty() ? nullptr : rendering.data(),
                              .swapchainCount = 1u,
                              .pSwapchains = std::addressof(swapchain.handle),
                              .pImageIndices = std::addressof(present.index),
                              .pResults = nullptr };

    if (VkResult result = vkQueuePresentKHR(present.handle, std::addressof(info)); result != VK_SUCCESS)
    {
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            rebuild_and_present(present, rendering, swapchain.handle);
        }
        else
        {
            LOG_FATAL("Unexpected error when attempting to present image: {}", vk::err_to_str(result));
        }
    }
}
void Presenter::rebuild_and_present(const vk::QueueView& present, const std::vector<VkSemaphore>& rendering, VkSwapchainKHR oldSwapchain)
{
    m_Swapchain = vk::Swapchain{ m_Device, m_PhysDevice, m_Surface, oldSwapchain };
    vk::SwapchainRef swapchain = m_Swapchain.handle();
    VkPresentInfoKHR info = { .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                              .pNext = nullptr,
                              .waitSemaphoreCount = static_cast<uint32_t>(rendering.size()),
                              .pWaitSemaphores = rendering.empty() ? nullptr : rendering.data(),
                              .swapchainCount = 1u,
                              .pSwapchains = std::addressof(swapchain.handle),
                              .pImageIndices = std::addressof(present.index),
                              .pResults = nullptr };
    VK_CHECK(vkQueuePresentKHR(present.handle, std::addressof(info)), "Presentation failed after rebuilding Swapchain..");
}
}	// namespace odin::graphics