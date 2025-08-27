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
struct PipelineLayoutKey
{
	std::vector<std::uint64_t> descSetLayoutHashes;
	std::vector<VkPushConstantRange> pushConstants;
	VkPipelineLayoutCreateFlags flags;
	[[nodiscard]] bool operator==(const PipelineLayoutKey& key) const = default;
	[[nodiscard]] bool operator!=(const PipelineLayoutKey& key) const = default;
};
struct PipelineLayoutKeyHasher : public common::SplitMix64<PipelineLayoutKeyHasher>
{
	[[nodiscard]] constexpr std::uint64_t operator()(const PipelineLayoutKey& key) const noexcept
	{
		std::uint64_t hash = key.descSetLayoutHashes.size() + key.pushConstants.size();

		for (auto&& h : key.descSetLayoutHashes)
		{
			hash ^= splitmix64(h) + (hash << 5) + (hash >> 3);
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
}	// namespace odin::graphics::registry::pipeline