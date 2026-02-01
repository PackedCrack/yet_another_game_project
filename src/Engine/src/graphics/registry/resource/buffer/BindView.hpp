//
// Created by qwerty on 07/09/2025.
//
#pragma once

#include <vulkan/vulkan.h>
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
//
//
constexpr VkDescriptorType to_descriptor_type(BindType type)
{
    switch (type)
    {
    case BindType::storage:
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    case BindType::dynamicStorage:
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
    case BindType::dynamicUniform:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    }
}
}    // namespace odin::graphics::registry::resource::buffer
