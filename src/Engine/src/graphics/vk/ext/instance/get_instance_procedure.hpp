//
// Created by qwerty on 02/10/2025.
//
#pragma once


#include "../../Instance.hpp"
// std
#include <string_view>
// vulkan
#include <vulkan/vulkan.h>
//
//
namespace odin::graphics::vk::ext::instance
{
template<typename func_t>
[[nodiscard]] func_t get_procedure_address(InstanceRef instance, std::string_view procedure)
{
    return reinterpret_cast<func_t>(vkGetInstanceProcAddr(instance.handle, procedure.data()));
}
}    //namespace odin::graphics::vk::ext::instance
