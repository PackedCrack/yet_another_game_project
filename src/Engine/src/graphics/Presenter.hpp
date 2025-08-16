//
// Created by qwerty on 15/08/2025.
//
#pragma once

#include "vk/Device.hpp"
#include "vk/PhysicalDevice.hpp"
#include "vk/Swapchain.hpp"
#include "vk/Surface.hpp"
#include "vk/synchronization/Semaphore.hpp"
//
//
namespace odin::graphics
{
class Presenter
{
public:
    Presenter(const vk::Device& device, const vk::PhysicalDevice& physDevice, vk::Surface surface);
public:
    [[nodiscard]] std::optional<vk::resource::ImageViewRef> acquire_color_attachment(vk::synchronization::SemaphoreRef imageAvailable);
    [[nodiscard]] bool present(const vk::QueueView& present, vk::synchronization::SemaphoreRef renderingFinished);
private:
    void rebuild(VkSwapchainKHR oldSwapchain);
    [[nodiscard]] std::optional<vk::resource::ImageViewRef> rebuild_and_acquire(vk::synchronization::SemaphoreRef imageAvailable);
private:
    vk::DeviceRef m_Device;
    vk::PhysicalDeviceRef m_PhysDevice;
    vk::Surface m_Surface;
    vk::Swapchain m_Swapchain;
    std::optional<vk::Swapchain::AcquiredImage> m_ColorAttachment;
};
}    // namespace odin::graphics
