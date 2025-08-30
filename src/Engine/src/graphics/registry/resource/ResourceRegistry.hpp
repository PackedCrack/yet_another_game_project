//
// Created by qwerty on 26/08/2025.
//
#pragma once

#include "shader/ShaderRegistry.hpp"
#include "../../vk/Device.hpp"
//
//
namespace odin::graphics::registry::resource
{
class ResourceRegistry
{
    using mutex_t = std::mutex;
    using Mutex = std::unique_ptr<mutex_t>;
public:
    ResourceRegistry(vk::DeviceRef device);
public:
    [[nodiscard]] shader::ShaderHandle shader(std::string_view filename);
private:
    std::unique_ptr<shader::ShaderRegistry> m_pShaders;
    Mutex m_pMutex;
};
}    // namespace odin::graphics::registry::resource
