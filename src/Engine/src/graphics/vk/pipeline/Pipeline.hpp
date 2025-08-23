//
// Created by qwerty on 17/08/2025.
//
#pragma once

#include "../CommandBuffer.hpp"
#include "../Device.hpp"
#include "../vulkan_defines.hpp"
//
//
namespace odin::graphics::vk::pipeline
{
template<typename derived_t>
class Pipeline
{
public:
    template<typename... make_pipeline_args_t>
    Pipeline(DeviceRef device, make_pipeline_args_t&&... args)
        : m_Pipeline{ VK_NULL_HANDLE }
        , m_Device{ device }
    {
        m_Pipeline = static_cast<derived_t*>(this)->make_pipeline(m_Device, std::forward<make_pipeline_args_t>(args)...);
    }
    ~Pipeline()
    {
        if (m_Pipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(m_Device.handle, m_Pipeline, nullptr);
        }
    }
    Pipeline(const Pipeline& other) = delete;
    Pipeline(Pipeline&& other) noexcept
        : m_Pipeline{ VK_NULL_HANDLE }
        , m_Device{ other.m_Device }
    {
        std::swap(m_Pipeline, other.m_Pipeline);
    }
    Pipeline& operator=(const Pipeline& other) = delete;
    Pipeline& operator=(Pipeline&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            std::swap(m_Pipeline, other.m_Pipeline);
            m_Device = other.m_Device;
        }

        return *this;
    }
public:
    void bind(CommandBufferRef cmdBuffer) const
    {
        VkPipelineBindPoint bindPoints = static_cast<derived_t*>(this)->bind_point();
        vkCmdBindPipeline(cmdBuffer.handle, bindPoints, m_Pipeline);
    }
protected:
    VkPipeline m_Pipeline = VK_NULL_HANDLE;
    DeviceRef m_Device;
};
}    // namespace odin::graphics::vk::pipeline
