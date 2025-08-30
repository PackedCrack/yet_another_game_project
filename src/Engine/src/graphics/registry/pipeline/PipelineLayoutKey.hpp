//
// Created by qwerty on 27/08/2025.
//
#pragma once


#include "descriptors/DescriptorSetLayoutKey.hpp"
// common
#include <SplitMix64.hpp>
// vulkan
#include <vulkan/vulkan.h>
//
//
namespace odin::graphics::registry::pipeline
{
struct PipelineLayoutKey
{
    std::vector<descriptors::DescriptorSetLayoutKey> descriptorLayoutKeys;
    std::vector<VkPushConstantRange> pushConstants;
    VkPipelineLayoutCreateFlags flags;
    [[nodiscard]] constexpr bool operator==(const PipelineLayoutKey& key) const
    {
        if (flags != key.flags)
        {
            return false;
        }
        if (pushConstants.size() != key.pushConstants.size())
        {
            return false;
        }
        if (descriptorLayoutKeys != key.descriptorLayoutKeys)
        {
            return false;
        }
        for (std::size_t i = 0; i < pushConstants.size(); ++i)
        {
            if (pushConstants[i].offset != key.pushConstants[i].offset)
            {
                return false;
            }
            if (pushConstants[i].size != key.pushConstants[i].size)
            {
                return false;
            }
            if (pushConstants[i].stageFlags != key.pushConstants[i].stageFlags)
            {
                return false;
            }
        }
        return true;
    };
    [[nodiscard]] constexpr bool operator!=(const PipelineLayoutKey& key) const { return !(*this == key); }
};
struct PipelineLayoutKeyHasher : public common::SplitMix64<PipelineLayoutKeyHasher>
{
    [[nodiscard]] inline std::uint64_t operator()(const PipelineLayoutKey& key) const noexcept
    {
        std::uint64_t hash = key.descriptorLayoutKeys.size() + key.pushConstants.size();

        descriptors::DescriptorSetLayoutKeyHasher hasher{};
        for (auto&& k : key.descriptorLayoutKeys)
        {
            hash ^= splitmix64(hasher(k)) + (hash << 5) + (hash >> 3);
        }
        for (auto&& range : key.pushConstants)
        {
            hash ^= splitmix64(range.offset) + (hash << 5) + (hash >> 3);
            hash ^= splitmix64(range.size) + (hash << 5) + (hash >> 3);
            hash ^= splitmix64(static_cast<std::uint64_t>(range.stageFlags)) + (hash << 5) + (hash >> 3);
        }
        hash ^= splitmix64(static_cast<std::uint64_t>(key.flags)) + (hash << 5) + (hash >> 3);

        return hash;
    }
};
}    // namespace odin::graphics::registry::pipeline
