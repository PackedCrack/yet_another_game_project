//
// Created by qwerty on 15/08/2025.
//
#pragma once

#include "FrameHandler.hpp"
#include "vk/Device.hpp"
#include "vk/PhysicalDevice.hpp"
#include "vk/Swapchain.hpp"
#include "vk/Surface.hpp"
#include "vk/synchronization/Semaphore.hpp"
#include "ColorAttachment.hpp"
//
//
namespace odin::graphics
{
class Presenter
{
public:
    Presenter(const vk::Device& device, const vk::PhysicalDevice& physDevice, vk::Surface surface, const FrameHandler& frameHandler);
public:
    [[nodiscard]] std::optional<ColorAttachment> acquire_color_attachment(vk::synchronization::SemaphoreRef imageAvailable);
    [[nodiscard]] bool present(const vk::QueueView& present, vk::synchronization::SemaphoreRef renderingFinished);
private:
    void rebuild(VkSwapchainKHR oldSwapchain);
    [[nodiscard]] std::optional<ColorAttachment> rebuild_and_acquire(vk::synchronization::SemaphoreRef imageAvailable);
private:
    std::uint32_t m_FramesInFlight;
    vk::DeviceRef m_Device;
    vk::PhysicalDeviceRef m_PhysDevice;
    vk::Surface m_Surface;
    vk::Swapchain m_Swapchain;
    std::optional<vk::Swapchain::AcquiredImage> m_ColorAttachment;
};
}    // namespace odin::graphics
