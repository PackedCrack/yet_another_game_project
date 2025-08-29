//
// Created by qwerty on 29/07/2025.
//
#include "ShaderRegistry.hpp"
//
//
namespace odin::graphics::registry::resource::shader
{
// Handles
ShaderHandle::ShaderHandle(std::shared_ptr<ShaderSlot> pSlot)
    : Handle<ShaderHandle, ShaderSlot, vk::resource::ShaderModule>(std::move(pSlot))
{}
//
//
ShaderRegistry::ShaderRegistry()
    : m_Shaders{}
    , m_pMutex{ std::make_unique<mutex_t>() }
{}
ShaderHandle ShaderRegistry::shader(const std::filesystem::path& filepath)
{
    // std::string stage = filepath.extension().string();

    auto pSlot = shader_slot(filepath);

    std::shared_ptr<const ShaderResource> pShader = std::atomic_load(std::addressof(pSlot->pResource));
    if (pShader == nullptr)
    {
        std::lock_guard lock{ pSlot->initMutex };

        pShader = std::atomic_load(std::addressof(pSlot->pResource));
        if (pShader == nullptr)
        {
            pShader = std::make_shared<const ShaderResource>(filepath);
            std::atomic_store(std::addressof(pSlot->pResource), std::move(pShader));
        }
    }

    return ShaderHandle{ std::move(pSlot) };
}
std::shared_ptr<ShaderSlot> ShaderRegistry::shader_slot(const std::filesystem::path& filepath)
{
    std::unique_lock lock{ *m_pMutex };

    auto [kvPair, emplaced] = m_Shaders.try_emplace(filepath, std::weak_ptr<ShaderSlot>{});
    if (std::shared_ptr<ShaderSlot> pSlot = kvPair->second.lock(); pSlot != nullptr)
    {
        return pSlot;
    }

    auto pSlot = std::make_shared<ShaderSlot>();
    kvPair->second = pSlot->weak_from_this();

    return pSlot;
}
}    // namespace odin::graphics::registry::resource::shader
