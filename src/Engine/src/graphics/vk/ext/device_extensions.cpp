//
// Created by qwerty on 02/10/2025.
//
#include "device_extensions.hpp"

#include "device/extended_dynamic_state3.hpp"
//
//
namespace odin::graphics::vk::ext
{
void load_device_extensions(DeviceRef device)
{
    device::load_extended_dynamic_state3(device);
}
}    //namespace odin::graphics::vk::ext
