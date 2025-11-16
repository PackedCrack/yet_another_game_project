//
// Created by qwerty on 27/08/2025.
//
#pragma once

#include "Request.hpp"
#include "../resource/shader/ShaderHandle.hpp"
//
//
namespace odin::graphics::registry::pipeline
{
enum class ShaderStage
{
    vertex,
    fragment,
    compute
};
enum class DescriptorType
{
    sampler,
    storageImage,
    sampledImage,
    storageBuffer,
    dynamicStorageBuffer,
    uniformBuffer,
    dynamicUniformBuffer
};
class RequestBuilder
{
public:
    RequestBuilder& add_vertex_shader(resource::shader::ShaderHandle shader);
    RequestBuilder& add_fragment_shader(resource::shader::ShaderHandle shader);
    RequestBuilder& add_compute_shader(resource::shader::ShaderHandle shader);
    RequestBuilder& add_color_format(VkFormat format);
    RequestBuilder& add_depth_format(VkFormat format);
    RequestBuilder& add_stencil_format(VkFormat format);
    RequestBuilder& add_polygon_mode(VkPolygonMode mode);
    RequestBuilder& add_msaa_sample_count(VkSampleCountFlagBits samples);
    template<typename... stage_t>
    requires(std::same_as<std::remove_cvref_t<stage_t>, ShaderStage> && ...)
    RequestBuilder& add_descriptor_layout(std::uint32_t setID, std::uint32_t bindingID, DescriptorType type, stage_t... stage)
    {
        add_descriptor_layout(setID, bindingID, type, false, std::forward<stage_t>(stage)...);
        return *this;
    }
    // clang-format off
    template<typename... stage_t>
    requires(std::same_as<std::remove_cvref_t<stage_t>, ShaderStage> && ...)
    RequestBuilder& add_descriptor_layout(std::uint32_t setID, 
                                          std::uint32_t bindingID,
                                          DescriptorType type, 
                                          bool updateAfterBind = false, 
                                          stage_t... stage)
    {
        DescriptorRequest& binding = add_descriptor_request(setID, bindingID);
        binding.bindingID = bindingID;
        binding.type = to_vk_desc_type(type);
        binding.numDescriptors = 1;    // always 1 now..
        binding.stages = to_vk_shader_stage(std::forward<stage_t>(stage)...);
        binding.flags = VK_NO_FLAGS;
        if (updateAfterBind)
        {
            binding.flags = VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        }

        return *this;
    }
    // clang-format on
    template<typename... stage_t>
    requires(std::same_as<std::remove_cvref_t<stage_t>, ShaderStage> && ...)
    RequestBuilder& add_push_constant_range(std::uint32_t offset, std::uint32_t size, stage_t... stage)
    {
        if (!m_Request.pushConstants.has_value())
        {
            m_Request.pushConstants = std::make_optional<std::vector<VkPushConstantRange>>();
        }

        VkPushConstantRange range{};
        range.offset = offset;
        range.size = size;
        range.stageFlags = to_vk_shader_stage(std::forward<stage_t>(stage)...);

        m_Request.pushConstants->push_back(range);

        return *this;
    }
    [[nodiscard]] Request build();
private:
    [[nodiscard]] std::map<std::uint32_t, DescriptorRequest>& descriptor_set_bindings(std::uint32_t setID);
    [[nodiscard]] DescriptorRequest& add_descriptor_request(std::uint32_t setID, std::uint32_t bindingID);
    template<typename... stage_t>
    requires(std::same_as<std::remove_cvref_t<stage_t>, ShaderStage> && ...)
    [[nodiscard]] VkShaderStageFlags to_vk_shader_stage(stage_t... stage)
    {
        VkShaderStageFlags stages{};
        auto store_stage = [&stages](ShaderStage stage)
        {
            switch (stage)
            {
            case ShaderStage::vertex:
                stages |= VK_SHADER_STAGE_VERTEX_BIT;
                break;
            case ShaderStage::fragment:
                stages |= VK_SHADER_STAGE_FRAGMENT_BIT;
                break;
            case ShaderStage::compute:
                stages |= VK_SHADER_STAGE_COMPUTE_BIT;
                break;
            }
        };
        (store_stage(stage), ...);

        return stages;
    }
    [[nodiscard]] VkDescriptorType to_vk_desc_type(DescriptorType type);
private:
    Request m_Request;
    std::map<std::uint32_t, std::map<std::uint32_t, DescriptorRequest>> m_DescriptorRequests;
};
}    // namespace odin::graphics::registry::pipeline
