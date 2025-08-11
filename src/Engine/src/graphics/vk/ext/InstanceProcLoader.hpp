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
    [[nodiscard]] func_t get_procedure_address(std::reference_wrapper<const Instance> instance, std::string_view procedure)
    {
        const Instance& i = instance.get();
        return reinterpret_cast<func_t>(vkGetInstanceProcAddr(i.handle(), procedure.data()));
    }
};
}    //namespace odin::graphics::vk::ext
