#pragma once
//
//
#include "Instance.hpp"
#include "PhysicalDevice.hpp"
//
//
namespace odin::graphics::vk
{
class QueueFamilies;
struct DeviceView
{
    VkDevice device;
};
class Device
{
public:
    Device(const PhysicalDevice& gpu, QueueFamilies& queueFamilies);
    ~Device();
    Device(const Device& other) = delete;
    Device(Device&& other) noexcept;
    Device& operator=(const Device& other) = delete;
    Device& operator=(Device&& other) noexcept;
public:
    [[nodiscard]] DeviceView view() const;
    [[nodiscard]] VkQueue get_queue_handle(std::uint32_t familyIndex) const;
private:
    VkDevice m_Device;
};
}    // namespace odin::graphics::vk
