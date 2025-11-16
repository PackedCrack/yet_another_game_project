//
// Created by qwerty on 07/09/2025.
//
#pragma once

#include "../../../vk/resource/DynamicBuffer.hpp"
//
//
namespace odin::graphics::registry::resource::buffer
{
enum class BindType
{
    storage,
    dynamicStorage,
    dynamicUniform
};
struct BindView
{
    BindType type;
    VkBuffer handle;
    VkDeviceSize offset;
    VkDeviceSize range;
};
}    // namespace odin::graphics::registry::resource::buffer
