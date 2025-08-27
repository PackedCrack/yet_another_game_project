//
// Created by qwerty on 26/07/2025.
//
#pragma once

#include "ShaderSlot.hpp"
#include "ShaderHandle.hpp"
//
//
namespace odin::graphics::registry::resource
{
class ResourceRegistry
{
    using mutex_t = std::mutex;
    using Mutex = std::unique_ptr<mutex_t>;
    using ShaderResource = vk::resource::ShaderModule;
    using ShaderRegistry = std::unordered_map<std::filesystem::path, std::weak_ptr<ShaderSlot>>;
public:
    ResourceRegistry();
public:
    [[nodiscard]] ShaderHandle shader(const std::filesystem::path& filepath);
private:
    [[nodiscard]] std::shared_ptr<ShaderSlot> shader_slot(const std::filesystem::path& filepath);
private:
    ShaderRegistry m_Shaders;
    Mutex m_pMutex;
};
}    // namespace odin::graphics::registry::resource
