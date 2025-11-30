//
// Created by qwerty on 02/10/2025.
//
#include "instance_extensions.hpp"

#include "instance/debug_utils.hpp"
//
//
namespace odin::graphics::vk::ext
{
void load_instance_extensions(InstanceRef instance, bool loadDebugUtils)
{
    if (loadDebugUtils)
    {
        instance::load_debug_utils(instance);
    }
}
}    //namespace odin::graphics::vk::ext
