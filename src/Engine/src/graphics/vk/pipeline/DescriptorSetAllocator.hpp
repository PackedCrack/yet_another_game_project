//
// Created by qwerty on 25/08/2025.
//
#pragma once


#include "DescriptorPool.hpp"
#include "DescriptorSetLayout.hpp"
#include "../Device.hpp"
//
//
namespace odin::graphics::vk::pipeline
{
class DescriptorSetAllocator
{
public:
    DescriptorSetAllocator(DeviceRef device);
    [[nodiscard]] VkDescriptorSet alloc(const DescriptorSetLayout& setLayout);
private:
    DescriptorPool m_Pool;
    DescriptorPool m_PoolWithUAB;
    DeviceRef m_Device;
};
}    // namespace odin::graphics::vk::pipeline
