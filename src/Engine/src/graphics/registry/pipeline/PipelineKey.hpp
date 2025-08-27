//
// Created by qwerty on 27/07/2025.
//
#pragma once

// common
#include <SplitMix64.hpp>
// vulkan
#include <vulkan/vulkan.h>
//
//
namespace odin::graphics::registry::pipeline
{
struct PipelineKey
{
    std::uint64_t vsHash;
    std::uint64_t fsHash;
    std::uint64_t csHash;

    std::uint64_t pipelineLayoutHash;

    std::optional<std::vector<VkFormat>> colorFormats{};
    std::optional<VkFormat> depthFormat;
    std::optional<VkFormat> stencilFormat;
    std::optional<VkPolygonMode> polygon;
    std::optional<VkSampleCountFlagBits> samples;

    [[nodiscard]] bool operator==(const PipelineKey& key) const = default;
    [[nodiscard]] bool operator!=(const PipelineKey& key) const = default;
};
struct PipelineKeyHasher : public common::SplitMix64<PipelineKeyHasher>
{
    constexpr std::uint64_t operator()(const PipelineKey& key) const noexcept
    {
        std::uint64_t hash = 11400'71481'93231'98549;

        hash ^= splitmix64(key.vsHash);
        hash ^= splitmix64(key.fsHash);
        hash ^= splitmix64(key.csHash);
        hash ^= splitmix64(key.pipelineLayoutHash);

        if (key.colorFormats)
        {
            for (auto&& format : key.colorFormats.value())
            {
                hash ^= (splitmix64(static_cast<std::uint64_t>(format)) + (hash << 3) + (hash >> 13));
            }
        }

        hash ^= splitmix64(static_cast<std::uint64_t>(key.depthFormat.value_or(0)));
        hash ^= splitmix64(static_cast<std::uint64_t>(key.stencilFormat.value_or(0)));
        hash ^= splitmix64(static_cast<std::uint64_t>(key.polygon.value_or(0)));
        hash ^= splitmix64(static_cast<std::uint64_t>(key.samples.value_or(0)));

        return hash;
    }
};
}    // namespace odin::graphics::registry::pipeline
