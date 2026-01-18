//
// Created by qwerty on 27/08/2025.
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
    [[nodiscard]] bool operator==(const PipelineKey& key) const
    {
        auto path = [](const auto& opt) -> std::optional<std::filesystem::path>
        {
            if (!opt)
            {
                return std::nullopt;
            }
            return opt->acquire()->filepath();
        };

        if (path(vs) == path(key.vs))
        {
            if (path(fs) == path(key.fs))
            {
                if (path(cs) == path(key.cs))
                {
                    return true;
                }
            }
        }

        return false;
    }
    [[nodiscard]] bool operator!=(const PipelineKey& key) const
    {
        auto path = [](const auto& opt) -> std::optional<std::filesystem::path>
        {
            if (!opt)
            {
                return std::nullopt;
            }
            return opt->acquire()->filepath();
        };

        if (path(vs) != path(key.vs))
        {
            return true;
        }
        if (path(fs) != path(key.fs))
        {
            return true;
        }
        if (path(cs) != path(key.cs))
        {
            return true;
        }

        return false;
    }
};
struct PipelineKeyHasher : public common::SplitMix64<PipelineKeyHasher>
{
    inline std::uint64_t operator()(const PipelineKey& key) const noexcept
    {
        std::uint64_t hash = 11400'71481'93231'98549;

        if (key.vs)
        {
            hash ^= splitmix64(key.vs.value().id());
        }
        if (key.fs)
        {
            hash ^= splitmix64(key.fs.value().id());
        }
        if (key.cs)
        {
            hash ^= splitmix64(key.cs.value().id());
        }

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

        LOG_INFO("Created pipeline hash: 0x{:X}", hash);
        return hash;
    }
};
}    // namespace odin::graphics::registry::pipeline
