//
// Created by qwerty on 26/08/2025.
//
#include "ShaderModule.hpp"


#include "../vulkan_defines.hpp"
// common
#include <common.hpp>
//
//
namespace
{
[[nodiscard]] VkShaderModuleCreateInfo make_create_info(std::span<std::uint32_t> fileContent)
{
    VkShaderModuleCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = VK_NO_FLAGS;
    info.codeSize = fileContent.size();
    info.pCode = fileContent.data();

    return info;
}
}    // namespace
namespace odin::graphics::vk::resource
{
ShaderModule::ShaderModule(DeviceRef device, std::filesystem::path filepath)
    : m_Module{ VK_NULL_HANDLE }
    , m_Device{ device }
    , m_Filepath{ std::move(filepath) }
{
    std::fstream file{ m_Filepath, std::ios::binary | std::ios::in };
    if (!file.is_open())
    {
        LOG_FATAL("Failed to open shader: {}", filepath.string().c_str());
    }

    std::uint64_t size = std::filesystem::file_size(filepath);
    if (size == 0)
    {
        LOG_FATAL("Opened Shader {} is empty.", filepath.string().c_str());
    }
    if (size % 4 != 0)
    {
        LOG_WARN("The size of Shader {} is not divisible by 4.");
    }

    std::vector<std::uint32_t> fileContent(size);
    file.read(reinterpret_cast<char*>(fileContent.data()), size);

    VkShaderModuleCreateInfo info = make_create_info(common::to_span(fileContent));
    VK_CHECK(vkCreateShaderModule(m_Device.handle, std::addressof(info), nullptr, std::addressof(m_Module)),
             "Failed to create Shader Module.");
}
ShaderModule::~ShaderModule()
{
    if (m_Module != VK_NULL_HANDLE)
    {
        vkDestroyShaderModule(m_Device.handle, m_Module, nullptr);
    }
}
ShaderModule::ShaderModule(ShaderModule&& other) noexcept
    : m_Module{ VK_NULL_HANDLE }
    , m_Device{ other.m_Device }
    , m_Filepath{ std::move(other.m_Filepath) }
{
    std::swap(m_Module, other.m_Module);
}
ShaderModule& ShaderModule::operator=(ShaderModule&& other) noexcept
{
    if (this != std::addressof(other))
    {
        std::swap(m_Module, other.m_Module);
        std::swap(m_Device, other.m_Device);
        std::swap(m_Filepath, other.m_Filepath);
    }
    return *this;
}
ShaderModuleRef ShaderModule::handle() const
{
    ODIN_ASSERT(m_Module != VK_NULL_HANDLE);
    return ShaderModuleRef{ .handle = m_Module };
}
}    // namespace odin::graphics::vk::resource
