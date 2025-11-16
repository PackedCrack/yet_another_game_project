//
// Created by qwerty on 26/08/2025.
//
#pragma once

#include "../../Slot.hpp"
#include "../../../vk/resource/ShaderModule.hpp"
//
//
namespace odin::graphics::registry::resource::shader
{
struct ShaderSlot : public Slot<ShaderSlot, vk::resource::ShaderModule>
{
    std::filesystem::path sourceFile;
    std::filesystem::file_time_type lastWrite;
    std::uint64_t hash;
};
}    // namespace odin::graphics::registry::resource::shader
