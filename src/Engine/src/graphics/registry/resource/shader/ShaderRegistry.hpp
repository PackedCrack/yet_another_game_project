//
// Created by qwerty on 29/08/2025.
//
#pragma once

#include "ShaderSlot.hpp"
#include "ShaderHandle.hpp"
#include "../../../vk/Device.hpp"
//
//
namespace odin::graphics::registry::resource::shader
{
class ShaderRegistry
{
    using mutex_t = std::mutex;
    using Mutex = std::unique_ptr<mutex_t>;
    using Registry = std::unordered_map<std::filesystem::path, std::weak_ptr<ShaderSlot>>;
public:
    [[nodiscard]] static std::unique_ptr<ShaderRegistry> make(vk::DeviceRef device);
private:
    ShaderRegistry(vk::DeviceRef device);
public:
    [[nodiscard]] ShaderHandle shader(std::string_view filename);
private:
    [[nodiscard]] ShaderHandle make_handle(std::shared_ptr<ShaderSlot> pSlot, const std::filesystem::path& shaderSource);
    [[nodiscard]] std::shared_ptr<ShaderSlot> create_slot(const std::filesystem::path& shaderSource);
    [[nodiscard]] std::shared_ptr<ShaderSlot> slot(const std::filesystem::path& shaderSource);
    [[nodiscard]] std::shared_ptr<const vk::resource::ShaderModule> load_shader(const std::filesystem::path& shaderSource);
    [[nodiscard]] bool is_outdated(const std::shared_ptr<ShaderSlot>& pSlot);
    [[nodiscard]] std::function<void()> make_hot_reload_cb(std::shared_ptr<ShaderSlot> pSlot, std::filesystem::path shaderSource);
    void refresh_slot(std::shared_ptr<ShaderSlot>& pSlot, const std::filesystem::path& shaderSource);
    void load_and_store_shader(std::shared_ptr<ShaderSlot>& pSlot, const std::filesystem::path& shaderSource);
private:
    vk::DeviceRef m_Device;
    Registry m_Shaders;
    Mutex m_pMutex;
};
}    // namespace odin::graphics::registry::resource::shader
