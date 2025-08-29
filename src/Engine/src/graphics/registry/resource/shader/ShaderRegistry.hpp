//
// Created by qwerty on 29/07/2025.
//
#pragma once

#include "ShaderSlot.hpp"
#include "ShaderHandle.hpp"
//
//
namespace odin::graphics::registry::resource::shader
{
class ShaderRegistry
{
    using mutex_t = std::mutex;
    using Mutex = std::unique_ptr<mutex_t>;
    using ShaderResource = vk::resource::ShaderModule;
    using Registry = std::unordered_map<std::filesystem::path, std::weak_ptr<ShaderSlot>>;
public:
    ShaderRegistry();
public:
    [[nodiscard]] ShaderHandle shader(const std::filesystem::path& filepath);
private:
    [[nodiscard]] std::shared_ptr<ShaderSlot> shader_slot(const std::filesystem::path& filepath);
private:
    Registry m_Shaders;
    Mutex m_pMutex;
};
}    // namespace odin::graphics::registry::resource::shader
