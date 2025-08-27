//
// Created by qwerty on 26/07/2025.
//
#pragma once

#include "../Slot.hpp"
#include "../../vk/resource/ShaderModule.hpp"
//
//
namespace odin::graphics::registry::resource
{
struct ShaderSlot : public Slot<ShaderSlot, vk::resource::ShaderModule>
{};
}    // namespace odin::graphics::registry::resource
