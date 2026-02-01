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
    uniformBuffer,
    storageBuffer,
    dynamicStorageBuffer,
    dynamicUniformBuffer,
    sampler,
    imageSampler,
    sampledImage,
    storageImage,
    uniformTexel,
    storageTexel,
    inputAttachment
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
    case BindType::uniformBuffer:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case BindType::storageBuffer:
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    case BindType::dynamicStorageBuffer:
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
    case BindType::dynamicUniformBuffer:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    case BindType::sampler:
        return VK_DESCRIPTOR_TYPE_SAMPLER;
    case BindType::imageSampler:
        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    case BindType::sampledImage:
        return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    case BindType::storageImage:
        return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    case BindType::uniformTexel:
        return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    case BindType::storageTexel:
        return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
    case BindType::inputAttachment:
        return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    }
}
}    // namespace odin::graphics::registry::resource::buffer
