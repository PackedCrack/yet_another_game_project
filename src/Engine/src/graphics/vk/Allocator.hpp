#pragma once

#include "Device.hpp"
#include "Instance.hpp"
#include "PhysicalDevice.hpp"
#include "resource/Image.hpp"
//
//
namespace odin::graphics::vk
{
class Allocator : std::enable_shared_from_this<Allocator>
{
    class Impl;
public:
    Allocator(const Instance& instance, const PhysicalDevice& gpu, const Device& device);
public:
    [[nodiscard]] resource::Image create_image_cpu_only(const VkImageCreateInfo& info);
    [[nodiscard]] resource::Image create_image_gpu_only(const VkImageCreateInfo& info);
    [[nodiscard]] resource::Image create_image_cpu_to_gpu(const VkImageCreateInfo& info);
    void destroy_image(VkImage image, void* pAllocation) const;
private:
    std::unique_ptr<Impl> m_pImpl;
};
}    // namespace odin::graphics::vk
