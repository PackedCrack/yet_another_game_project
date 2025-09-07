//
// Created by qwerty on 27/08/2025.
//
#include "RequestBuilder.hpp"

// common
#include <common.hpp>
// Debug
#include <debug/Logger.hpp>
//
//
namespace odin::graphics::registry::pipeline
{
RequestBuilder& RequestBuilder::add_vertex_shader(resource::shader::ShaderHandle shader)
{
    m_Request.vs = std::make_optional(std::move(shader));
    return *this;
}
RequestBuilder& RequestBuilder::add_fragment_shader(resource::shader::ShaderHandle shader)
{
    m_Request.fs = std::make_optional(std::move(shader));
    return *this;
}
RequestBuilder& RequestBuilder::add_compute_shader(resource::shader::ShaderHandle shader)
{
    m_Request.cs = std::make_optional(std::move(shader));
    return *this;
}
RequestBuilder& RequestBuilder::add_color_format(VkFormat format)
{
    if (!m_Request.colorFormats)
    {
        m_Request.colorFormats = std::make_optional<std::vector<VkFormat>>();
    }
    m_Request.colorFormats->emplace_back(format);

    return *this;
}
RequestBuilder& RequestBuilder::add_depth_format(VkFormat format)
{
    m_Request.depthFormat = std::make_optional(format);
    return *this;
}
RequestBuilder& RequestBuilder::add_stencil_format(VkFormat format)
{
    m_Request.stencilFormat = std::make_optional(format);
    return *this;
}
RequestBuilder& RequestBuilder::add_polygon_mode(VkPolygonMode mode)
{
    m_Request.polygonMode = std::make_optional(mode);
    return *this;
}
RequestBuilder& RequestBuilder::add_msaa_sample_count(VkSampleCountFlagBits samples)
{
    m_Request.MsaaSamples = std::make_optional(samples);
    return *this;
}
Request RequestBuilder::build()
{
    for (auto&& [setID, set] : m_DescriptorRequests)
    {
        m_Request.DescLayoutRequirement.emplace_back();
        auto&& s = m_Request.DescLayoutRequirement.back();
        for (auto&& [bindingID, request] : set)
        {
            s.emplace_back(request);
        }
    }

    Request tmp = std::move(m_Request);
    m_Request = Request{};

    return tmp;
}
std::map<std::uint32_t, DescriptorRequest>& RequestBuilder::descriptor_set_bindings(std::uint32_t setID)
{
    ODIN_ASSERT(setID <= 7);    // Minimum # sets guarenteed by vulkan 1.4

    auto&& [it, emplaced] = m_DescriptorRequests.try_emplace(setID);
    return it->second;
}
DescriptorRequest& RequestBuilder::add_descriptor_request(std::uint32_t setID, std::uint32_t bindingID)
{
    std::map<std::uint32_t, DescriptorRequest>& bindings = descriptor_set_bindings(setID);
    ODIN_ASSERT(!bindings.contains(bindingID));

    auto&& [it, emplaced] = bindings.try_emplace(bindingID, DescriptorRequest{});
    ODIN_ASSERT(emplaced);

    return it->second;
}
VkDescriptorType RequestBuilder::to_vk_desc_type(DescriptorType type)
{
    switch (type)
    {
    case DescriptorType::sampler:
        return VK_DESCRIPTOR_TYPE_SAMPLER;
    case DescriptorType::storageImage:
        return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    case DescriptorType::sampledImage:
        return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    case DescriptorType::storageBuffer:
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    case DescriptorType::dynamicStorageBuffer:
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
    case DescriptorType::uniformBuffer:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case DescriptorType::dynamicUniformBuffer:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    }

    std::unreachable();
}
}    // namespace odin::graphics::registry::pipeline
