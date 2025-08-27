//
// Created by qwerty on 26/07/2025.
//
#pragma once

#include "ShaderSlot.hpp"
#include "../Handle.hpp"
#include "../../vk/resource/ShaderModule.hpp"
//
//
namespace odin::graphics::registry::resource
{
struct ShaderHandle : public Handle<ShaderHandle, ShaderSlot, vk::resource::ShaderModule>
{
    ShaderHandle(std::shared_ptr<ShaderSlot> pSlot);
};
}    // namespace odin::graphics::registry::resource
