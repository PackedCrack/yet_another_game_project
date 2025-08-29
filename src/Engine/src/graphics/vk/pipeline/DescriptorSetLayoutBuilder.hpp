//
// Created by qwerty on 25/08/2025.
//
#pragma once

#include "DescriptorSetLayout.hpp"
#include "../Device.hpp"
//
//
namespace odin::graphics::vk::pipeline
{
class DescriptorSetLayoutBuilder
{
    struct Binding
    {
        VkDescriptorSetLayoutBinding binding;
        VkDescriptorBindingFlags flags;
        friend inline bool operator<(const Binding& lhs, const Binding& rhs) { return lhs.binding.binding < rhs.binding.binding; }
    };
public:
    DescriptorSetLayoutBuilder& add_binding(std::uint32_t bindingID,
                                            VkDescriptorType type,
                                            std::uint32_t descriptorCount,
                                            VkShaderStageFlags stages,
                                            VkDescriptorBindingFlags bindFlags = VK_NO_FLAGS);
    DescriptorSetLayoutBuilder& add_runtime_descriptor_array(std::uint32_t bindingID,
                                                             VkDescriptorType type,
                                                             std::uint32_t descriptorCount,
                                                             VkShaderStageFlags stages,
                                                             bool partiallyBound = true,
                                                             bool updateAfterBind = true);
    [[nodiscard]] DescriptorSetLayout build(DeviceRef device);
private:
    void get_packed_bindings(std::vector<VkDescriptorSetLayoutBinding>* outBinding, std::vector<VkDescriptorBindingFlags>* outFlags) const;
private:
    std::set<Binding> m_Bindings;
};
}    // namespace odin::graphics::vk::pipeline
