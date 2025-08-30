//
// Created by qwerty on 29/08/2025.
//
#include "ShaderRegistry.hpp"

#include "ShaderCompiler.hpp"
#include "../../../../FilepathResolver.hpp"
// debug
#include <debug/debug_defines.hpp>
//
//
namespace
{
using namespace odin::graphics;
using namespace odin::graphics::registry::resource::shader;
//
//
[[nodiscard]] std::filesystem::path resolve_shader_filepath(std::string_view filename)
{
    auto& resolver = odin::FilepathResolver::get();
    return resolver.resolve_shader_path(filename);
}
[[nodiscard]] std::filesystem::path to_spirv_filepath(const std::filesystem::path& shaderSource)
{
    std::filesystem::path spirv = shaderSource;
    spirv = spirv.replace_extension(".spv");
    return spirv;
}
void compile_shader(const std::filesystem::path& shaderSource)
{
    ShaderCompiler compiler{ shaderSource, to_spirv_filepath(shaderSource) };
    compiler.compile();
}
void store_shader(std::shared_ptr<ShaderSlot>& pSlot,
                  std::shared_ptr<const vk::resource::ShaderModule>& pShader,
                  const std::filesystem::path& shaderSource)
{
    pSlot->sourceFile = shaderSource;
    pSlot->lastWrite = std::filesystem::last_write_time(shaderSource);
    pSlot->hash = pShader->hash();
    std::atomic_store(std::addressof(pSlot->pResource), std::move(pShader));
}
}    // namespace
namespace odin::graphics::registry::resource::shader
{
std::unique_ptr<ShaderRegistry> ShaderRegistry::make(vk::DeviceRef device)
{
    return std::make_unique<ShaderRegistry>(ShaderRegistry{ device });
}
ShaderRegistry::ShaderRegistry(vk::DeviceRef device)
    : m_Device{ device }
    , m_Shaders{}
    , m_pMutex{ std::make_unique<mutex_t>() }
{}
ShaderHandle ShaderRegistry::shader(std::string_view filename)
{
    std::filesystem::path shaderSource = resolve_shader_filepath(filename);
    if (!m_Shaders.contains(shaderSource))
    {
        std::lock_guard lock{ *m_pMutex };
        if (!m_Shaders.contains(shaderSource))
        {
            std::shared_ptr<ShaderSlot> pSlot = create_slot(shaderSource);
            return make_handle(std::move(pSlot), shaderSource);
        }
    }

    auto pSlot = slot(shaderSource);
    refresh_slot(pSlot, shaderSource);

    return make_handle(std::move(pSlot), shaderSource);
}
ShaderHandle ShaderRegistry::make_handle(std::shared_ptr<ShaderSlot> pSlot, const std::filesystem::path& shaderSource)
{
    ShaderHandle handle{ std::move(pSlot), make_hot_reload_cb(pSlot, shaderSource) };
    return handle;
}
std::shared_ptr<ShaderSlot> ShaderRegistry::create_slot(const std::filesystem::path& shaderSource)
{
    auto pSlot = std::make_shared<ShaderSlot>();
    refresh_slot(pSlot, shaderSource);
    m_Shaders.emplace(shaderSource, pSlot->weak_from_this());

    return pSlot;
}
std::shared_ptr<ShaderSlot> ShaderRegistry::slot(const std::filesystem::path& shaderSource)
{
    std::weak_ptr<ShaderSlot> wpSlot = m_Shaders.at(shaderSource);
    std::shared_ptr<ShaderSlot> pSlot = wpSlot.lock();

    if (pSlot == nullptr)
    {
        std::unique_lock lock{ *m_pMutex };
        if (pSlot == nullptr)
        {
            pSlot = create_slot(shaderSource);
        }
    }

    return pSlot;
}
std::shared_ptr<const vk::resource::ShaderModule> ShaderRegistry::load_shader(const std::filesystem::path& shaderSource)
{
    if (std::filesystem::exists(shaderSource))
    {
        ODIN_ASSERT(std::filesystem::is_regular_file(shaderSource));
        compile_shader(shaderSource);
    }

    std::filesystem::path spirv = to_spirv_filepath(shaderSource);
    if (std::filesystem::exists(spirv))
    {
        ODIN_ASSERT(std::filesystem::is_regular_file(spirv));
        return std::make_shared<vk::resource::ShaderModule>(m_Device, std::move(spirv));
    }

    LOG_FATAL("Unable to find shader file: {}", spirv.string().c_str());
}
bool ShaderRegistry::is_outdated(const std::shared_ptr<ShaderSlot>& pSlot)
{
    using ShaderModule = vk::resource::ShaderModule;

    std::filesystem::file_time_type currentTime = std::filesystem::last_write_time(pSlot->sourceFile);
    return pSlot->lastWrite != currentTime;
}
std::function<void()> ShaderRegistry::make_hot_reload_cb(std::shared_ptr<ShaderSlot> pSlot, std::filesystem::path shaderSource)
{
    // this should probably be weak ptr since Handle holders keeps the slots alive.
    // Could happen that handle owners outlive the Registry if its stored as unique_ptr
    return [this, slot = std::move(pSlot), shaderSource = std::move(shaderSource)]() mutable
    {
        if (is_outdated(slot))
        {
            std::shared_ptr<const vk::resource::ShaderModule> pShader = load_shader(shaderSource);
            if (slot->hash != pShader->hash())
            {
                store_shader(slot, pShader, shaderSource);
            }
            else
            {
                slot->lastWrite = std::filesystem::last_write_time(shaderSource);
            }
        }
    };
}
void ShaderRegistry::load_and_store_shader(std::shared_ptr<ShaderSlot>& pSlot, const std::filesystem::path& shaderSource)
{
    std::shared_ptr<const vk::resource::ShaderModule> pShader = load_shader(shaderSource);
    store_shader(pSlot, pShader, shaderSource);
}
void ShaderRegistry::refresh_slot(std::shared_ptr<ShaderSlot>& pSlot, const std::filesystem::path& shaderSource)
{
    std::shared_ptr<const vk::resource::ShaderModule> pShader = std::atomic_load(std::addressof(pSlot->pResource));
    if (pShader == nullptr)
    {
        std::lock_guard initLock{ pSlot->initMutex };
        pShader = std::atomic_load(std::addressof(pSlot->pResource));
        if (pShader == nullptr)
        {
            load_and_store_shader(pSlot, shaderSource);
        }
    }
}
}    // namespace odin::graphics::registry::resource::shader
