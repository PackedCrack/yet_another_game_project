//
// Created by qwerty on 26/08/2025.
//
#include "ResourceRegistry.hpp"
//
//
namespace odin::graphics::registry::resource
{
ResourceRegistry::ResourceRegistry(vk::DeviceRef device)
    : m_pShaders{ shader::ShaderRegistry::make(device) }
    , m_pMutex{ std::make_unique<mutex_t>() }
{}
shader::ShaderHandle ResourceRegistry::shader(std::string_view filename)
{
    return m_pShaders->shader(filename);
}
}    // namespace odin::graphics::registry::resource
