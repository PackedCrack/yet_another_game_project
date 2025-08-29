//
// Created by qwerty on 27/07/2025.
//
#pragma once

#include "../resource/shader/ShaderHandle.hpp"
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
    std::optional<resource::shader::ShaderHandle> vs;
    std::optional<resource::shader::ShaderHandle> fs;
    std::optional<resource::shader::ShaderHandle> cs;

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
    inline std::uint64_t operator()(const PipelineKey& key) const noexcept
    {
        std::uint64_t hash = 11400'71481'93231'98549;

        // TODO:
        // Use ShaderHandle ID in the hash

        hash ^= splitmix64(key.pipelineLayoutHash);

        if (key.colorFormats)
        {
            for (auto&& format : key.colorFormats.value())
            {
                hash ^= (splitmix64(static_cast<std::uint64_t>(format)) + (hash << 3) + (hash >> 13));
            }
        }

        hash ^= splitmix64(static_cast<std::uint64_t>(key.depthFormat.value_or(VK_FORMAT_UNDEFINED)));
        hash ^= splitmix64(static_cast<std::uint64_t>(key.stencilFormat.value_or(VK_FORMAT_UNDEFINED)));
        hash ^= splitmix64(static_cast<std::uint64_t>(key.polygon.value_or(VK_POLYGON_MODE_MAX_ENUM)));
        hash ^= splitmix64(static_cast<std::uint64_t>(key.samples.value_or(VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM)));

        return hash;
    }
};
}    // namespace odin::graphics::registry::pipeline
