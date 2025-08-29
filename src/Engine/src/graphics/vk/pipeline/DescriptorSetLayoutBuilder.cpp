//
// Created by qwerty on 25/08/2025.
//
#include "DescriptorSetLayoutBuilder.hpp"

#include "../vulkan_defines.hpp"
// common
#include <common.hpp>
//
//
namespace
{
[[nodiscard]] VkDescriptorSetLayoutBindingFlagsCreateInfo make_flags_create_info(std::span<VkDescriptorBindingFlags> flags)
{
    VkDescriptorSetLayoutBindingFlagsCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    info.pNext = nullptr;
    info.pBindingFlags = flags.empty() ? nullptr : flags.data();
    info.bindingCount = static_cast<std::uint32_t>(flags.size());

    return info;
}
[[nodiscard]] VkDescriptorSetLayoutCreateInfo make_layout_create_info(std::span<VkDescriptorSetLayoutBinding> bindings,
                                                                      const VkDescriptorSetLayoutBindingFlagsCreateInfo& flagsInfo)
{
    VkDescriptorSetLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.pNext = flagsInfo.pBindingFlags == nullptr ? nullptr : std::addressof(flagsInfo);
    info.flags = VK_NO_FLAGS;
    info.bindingCount = static_cast<std::uint32_t>(bindings.size());
    info.pBindings = bindings.data();

    return info;
}
}    // namespace
namespace odin::graphics::vk::pipeline
{
DescriptorSetLayoutBuilder& DescriptorSetLayoutBuilder::add_binding(std::uint32_t bindingID,
                                                                    VkDescriptorType type,
                                                                    std::uint32_t descriptorCount,
                                                                    VkShaderStageFlags stages,
                                                                    VkDescriptorBindingFlags bindFlags)
{
    VkDescriptorSetLayoutBinding binding{};
    binding.binding = bindingID;
    binding.descriptorCount = descriptorCount;
    binding.descriptorType = type;
    binding.pImmutableSamplers = nullptr;
    binding.stageFlags = stages;

    Binding b{};
    b.binding = binding;
    b.flags = bindFlags;

    auto [it, emplaced] = m_Bindings.emplace(b);
    ODIN_ASSERT(emplaced);

    return *this;
}
DescriptorSetLayoutBuilder& DescriptorSetLayoutBuilder::add_runtime_descriptor_array(std::uint32_t bindingID,
                                                                                     VkDescriptorType type,
                                                                                     std::uint32_t descriptorCount,
                                                                                     VkShaderStageFlags stages,
                                                                                     bool partiallyBound,
                                                                                     bool updateAfterBind)
{
    VkDescriptorBindingFlags flags = VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;
    if (partiallyBound)
    {
        flags |= VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
    }
    if (updateAfterBind)
    {
        flags |= VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
    }
    return add_binding(bindingID, type, descriptorCount, stages, flags);
}
DescriptorSetLayout DescriptorSetLayoutBuilder::build(DeviceRef device)
{
    std::vector<VkDescriptorSetLayoutBinding> bindings{};
    std::vector<VkDescriptorBindingFlags> flags{};
    get_packed_bindings(std::addressof(bindings), std::addressof(flags));

    VkDescriptorSetLayoutBindingFlagsCreateInfo flagsInfo = make_flags_create_info(common::to_span(flags));
    VkDescriptorSetLayoutCreateInfo info = make_layout_create_info(common::to_span(bindings), flagsInfo);

    return DescriptorSetLayout{ device, info };
}
void DescriptorSetLayoutBuilder::get_packed_bindings(std::vector<VkDescriptorSetLayoutBinding>* outBinding,
                                                     std::vector<VkDescriptorBindingFlags>* outFlags) const
{
    for (auto&& binding : m_Bindings)
    {
        outBinding->emplace_back(binding.binding);
        outFlags->emplace_back(binding.flags);
    }
}
}    // namespace odin::graphics::vk::pipeline
