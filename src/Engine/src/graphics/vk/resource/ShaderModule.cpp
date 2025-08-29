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
    info.codeSize = fileContent.size() * sizeof(std::uint32_t);
    info.pCode = fileContent.data();

    return info;
}
[[nodiscard]] std::uint64_t hash_file_content(std::span<const std::uint32_t> fileContent)
{
    std::size_t result = fileContent.size();
    std::hash<std::uint32_t> hasher{};
    for (std::uint32_t DWORD : fileContent)
    {
        result ^= hasher(DWORD) + 11400'71481'93231'98549 + (result << 3) + (result >> 7);
    }

    return result;
}
}    // namespace
namespace odin::graphics::vk::resource
{
ShaderModule::ShaderModule(DeviceRef device, std::filesystem::path filepath)
    : m_Module{ VK_NULL_HANDLE }
    , m_Device{ device }
    , m_Filepath{ std::move(filepath) }
    , m_Hash{}
{
    std::fstream file{ m_Filepath, std::ios::binary | std::ios::in };
    if (!file.is_open())
    {
        LOG_FATAL("Failed to open shader: {}", m_Filepath.string().c_str());
    }

    std::uint64_t fileSize = std::filesystem::file_size(m_Filepath);
    if (fileSize == 0)
    {
        LOG_FATAL("Opened Shader {} is empty.", m_Filepath.string().c_str());
    }
    if (fileSize % 4 != 0)
    {
        LOG_WARN("The size of Shader {} is not divisible by 4.", fileSize);
    }

    std::vector<std::uint32_t> fileContent(fileSize / sizeof(std::uint32_t));
    file.read(reinterpret_cast<char*>(fileContent.data()), fileSize);

    VkShaderModuleCreateInfo info = make_create_info(common::to_span(fileContent));
    VK_CHECK(vkCreateShaderModule(m_Device.handle, std::addressof(info), nullptr, std::addressof(m_Module)),
             "Failed to create Shader Module.");

    m_Hash = hash_file_content(common::to_span(fileContent));
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
    , m_Hash{ other.m_Hash }
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
        std::swap(m_Hash, other.m_Hash);
    }
    return *this;
}
ShaderModuleRef ShaderModule::handle() const
{
    ODIN_ASSERT(m_Module != VK_NULL_HANDLE);
    return ShaderModuleRef{ .handle = m_Module };
}
std::uint64_t ShaderModule::hash() const
{
    ODIN_ASSERT(m_Hash != 0);
    return m_Hash;
}
std::filesystem::path ShaderModule::filepath() const
{
    return m_Filepath;
}
}    // namespace odin::graphics::vk::resource
