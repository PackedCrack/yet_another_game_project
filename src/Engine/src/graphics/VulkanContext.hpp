#pragma once

#include "../OdinInfo.hpp"
#include "vk/DebugMessenger.hpp"
#include "vk/Device.hpp"
#include "vk/Instance.hpp"
#include "vk/PhysicalDevice.hpp"
#include "vk/QueueFamilies.hpp"
#include "window/Window.hpp"
//
//
namespace odin::graphics
{
class VulkanContext
{
public:
    VulkanContext(const OdinInfo& info, const window::Window& window);
private:
    vk::Instance m_Instance;
    std::optional<vk::DebugMessenger> m_DebugMsg;
    vk::PhysicalDevice m_PhysicalDevice;
    vk::QueueFamilies m_Queues;
    vk::Device m_Device;
};
}    // namespace odin::graphics
