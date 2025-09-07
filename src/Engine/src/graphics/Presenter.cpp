//
// Created by qwerty on 15/08/2025.
//
#include "Presenter.hpp"

#include "vk/vulkan_defines.hpp"
//
//
namespace odin::graphics
{
Presenter::Presenter(const vk::Device& device, const vk::PhysicalDevice& physDevice, vk::Surface surface, const FrameHandler& frameHandler)
    : m_FramesInFlight{ frameHandler.in_flight_count() }
    , m_Device{ device.handle() }
    , m_PhysDevice{ physDevice.handle() }
    , m_Surface{ std::move(surface) }
    , m_Swapchain{ device, physDevice, m_Surface, m_FramesInFlight }
    , m_ColorAttachment{ std::nullopt }
{}
std::optional<ColorAttachment> Presenter::acquire_color_attachment(vk::synchronization::SemaphoreRef imageAvailable)
{
    using AcquiredImage = vk::Swapchain::AcquiredImage;
    using Error = vk::Swapchain::Error;

    std::expected<AcquiredImage, Error> aquired = m_Swapchain.acquire(imageAvailable.handle);
    if (aquired)
    {
        m_ColorAttachment = std::make_optional<AcquiredImage>(std::move(aquired.value()));
        return std::make_optional<ColorAttachment>(m_ColorAttachment.value(), m_Surface.current_extent(m_PhysDevice));
    }
    else
    {
        return rebuild_and_acquire(imageAvailable);
    }
}
bool Presenter::present(const vk::QueueView& present, vk::synchronization::SemaphoreRef graphicsFinished)
{
    ODIN_ASSERT(m_ColorAttachment);

    vk::SwapchainRef swapchain = m_Swapchain.handle();
    VkPresentInfoKHR info = { .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                              .pNext = nullptr,
                              .waitSemaphoreCount = 1,
                              .pWaitSemaphores = std::addressof(graphicsFinished.handle),
                              .swapchainCount = 1u,
                              .pSwapchains = std::addressof(swapchain.handle),
                              .pImageIndices = std::addressof(m_ColorAttachment->index),
                              .pResults = nullptr };

    if (VkResult result = vkQueuePresentKHR(present.handle, std::addressof(info)); result != VK_SUCCESS)
    {
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            rebuild(swapchain.handle);
            return false;
        }
        else
        {
            LOG_FATAL("Unexpected error when attempting to present image: {}", vk::err_to_str(result));
        }
    }

    m_ColorAttachment = std::nullopt;
    return true;
}
void Presenter::rebuild(VkSwapchainKHR oldSwapchain)
{
    m_Swapchain = vk::Swapchain{ m_Device, m_PhysDevice, m_Surface, oldSwapchain, m_FramesInFlight };
    m_ColorAttachment = std::nullopt;
}
std::optional<ColorAttachment> Presenter::rebuild_and_acquire(vk::synchronization::SemaphoreRef imageAvailable)
{
    using AcquiredImage = vk::Swapchain::AcquiredImage;
    using Error = vk::Swapchain::Error;

    vk::SwapchainRef swapchain = m_Swapchain.handle();
    rebuild(swapchain.handle);

    std::expected<AcquiredImage, Error> aquired = m_Swapchain.acquire(imageAvailable.handle);
    if (!aquired)
    {
        return std::nullopt;
    }

    m_ColorAttachment = std::make_optional<AcquiredImage>(std::move(aquired.value()));
    return std::make_optional<ColorAttachment>(m_ColorAttachment.value(), m_Surface.current_extent(m_PhysDevice));
}
}    // namespace odin::graphics
