//
// Created by qwerty on 26/07/2025.
//
#include "ShaderHandle.hpp"
//
//
namespace odin::graphics::registry::resource
{
ShaderHandle::ShaderHandle(std::shared_ptr<ShaderSlot> pSlot)
    : Handle<ShaderHandle, ShaderSlot, vk::resource::ShaderModule>(std::move(pSlot))
{}
}    // namespace odin::graphics::registry::resource
