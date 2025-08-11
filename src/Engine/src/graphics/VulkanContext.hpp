#pragma once

#include "../OdinInfo.hpp"
#include "vk/DebugMessenger.hpp"
#include "vk/Device.hpp"
#include "vk/Instance.hpp"
#include "vk/PhysicalDevice.hpp"
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
    vk::DebugMessenger m_DebugMsg;
    vk::Device m_Device;
    vk::PhysicalDevice m_PhysicalDevice;
};
}    // namespace odin::graphics
