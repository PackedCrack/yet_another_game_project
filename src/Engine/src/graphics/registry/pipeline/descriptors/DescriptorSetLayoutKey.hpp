//
// Created by qwerty on 28/07/2025.
//
#pragma once


// debug
#include <debug/debug_defines.hpp>
// common
#include <SplitMix64.hpp>
// vulkan
#include <vulkan/vulkan.h>
//
//
namespace odin::graphics::registry::pipeline::descriptors
{
struct Binding
{
    std::uint32_t binding;
    VkDescriptorType descriptorType;
    std::uint32_t descriptorCount;
    VkShaderStageFlags stageFlags;
    std::uint64_t immutableSamplerHash;
    VkDescriptorBindingFlags bindingFlags;

    [[nodiscard]] bool operator==(const Binding& key) const = default;
    [[nodiscard]] bool operator!=(const Binding& key) const = default;
};
struct DescriptorSetLayoutKey
{
    std::vector<Binding> bindings;
    [[nodiscard]] bool operator==(const DescriptorSetLayoutKey& key) const = default;
    [[nodiscard]] bool operator!=(const DescriptorSetLayoutKey& key) const = default;
};
struct DescriptorSetLayoutKeyHasher : public common::SplitMix64<DescriptorSetLayoutKeyHasher>
{
    [[nodiscard]] constexpr std::uint64_t operator()(const DescriptorSetLayoutKey& key) const noexcept
    {
#ifndef NDEBUG
        // Make sure the bindings are sorted when they get here
        if (key.bindings.size() > 1)
        {
            auto it = std::begin(key.bindings);
            auto nextIt = std::next(it);
            while (nextIt != std::end(key.bindings))
            {
                ODIN_ASSERT(it->binding < nextIt->binding);

                std::advance(it, 1);
                std::advance(nextIt, 1);
            }
        }

        // Update after bind cannot be used with dynamic buffers
        auto it = std::find_if(std::begin(key.bindings),
                               std::end(key.bindings),
                               [](const Binding& binding)
                               {
                                   bool updatedAfterBind = binding.bindingFlags & VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
                                   bool hasDynamic = binding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC ||
                                                     binding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;

                                   return updatedAfterBind && hasDynamic;
                               });
        ODIN_ASSERT(it == std::end(key.bindings));
#endif

        std::uint64_t hash = key.bindings.size();

        for (auto&& binding : key.bindings)
        {
            hash ^= splitmix64(binding.binding) + (hash << 11) + (hash >> 7);
            hash ^= splitmix64(static_cast<std::uint64_t>(binding.descriptorType)) + (hash << 3) + (hash >> 5);
            hash ^= splitmix64(binding.descriptorCount) + (hash << 2) + (hash >> 13);
            hash ^= splitmix64(static_cast<std::uint64_t>(binding.stageFlags)) + (hash << 7) + (hash >> 3);
            hash ^= splitmix64(binding.immutableSamplerHash) + (hash << 7) + (hash >> 3);
            if (binding.bindingFlags & VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT)
            {
                hash ^= splitmix64(11400'71481'93231'98549) + (hash << 13) + (hash >> 5);
            }
            if (binding.bindingFlags & VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT)
            {
                hash ^= splitmix64(11400'71481'93231'98549) + (hash << 3) + (hash >> 5);
            }
        }


        return hash;
    }
};
}    // namespace odin::graphics::registry::pipeline::descriptors
