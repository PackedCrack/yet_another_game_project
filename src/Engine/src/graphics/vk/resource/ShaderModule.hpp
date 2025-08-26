//
// Created by qwerty on 26/08/2025.
//
#pragma once

#include "../Device.hpp"
//
//
namespace odin::graphics::vk::resource
{
struct ShaderModuleRef
{
    VkShaderModule handle;
};
class ShaderModule
{
public:
    ShaderModule(DeviceRef device, std::filesystem::path filepath);
    ~ShaderModule();
    ShaderModule(const ShaderModule& other) = delete;
    ShaderModule(ShaderModule&& other) noexcept;
    ShaderModule& operator=(const ShaderModule& other) = delete;
    ShaderModule& operator=(ShaderModule&& other) noexcept;
public:
    [[nodiscard]] ShaderModuleRef handle() const;
private:
    VkShaderModule m_Module;
    DeviceRef m_Device;
    std::filesystem::path m_Filepath;
};
}    // namespace odin::graphics::vk::resource
