//
// Created by qwerty on 02/10/2025.
//
#pragma once


#include "../../Device.hpp"
// std
#include <string_view>
// vulkan
#include <vulkan/vulkan.h>
//
//
namespace odin::graphics::vk::ext::device
{
template<typename func_t>
[[nodiscard]] func_t get_procedure_address(DeviceRef device, std::string_view procedure)
{
    return reinterpret_cast<func_t>(vkGetDeviceProcAddr(device.handle, procedure.data()));
}
}    //namespace odin::graphics::vk::ext::device
