//
// Created by qwerty on 11/08/2025.
//
#pragma once

#include "../Instance.hpp"
// vulkan
#include <vulkan/vulkan.h>
// std
#include <string_view>
//
//
namespace odin::graphics::vk::ext
{
template<typename derived_t>
class InstanceProcLoader
{
protected:
    template<typename func_t>
    [[nodiscard]] func_t get_procedure_address(InstanceRef instance, std::string_view procedure)
    {
        return reinterpret_cast<func_t>(vkGetInstanceProcAddr(instance.handle, procedure.data()));
    }
};
}    //namespace odin::graphics::vk::ext
