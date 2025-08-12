#pragma once

#include "PhysicalDevice.hpp"
#include "Surface.hpp"
// vulkan
#include <vulkan/vulkan.h>
//
//
namespace odin::graphics::vk
{
class Device;
struct QueueView
{
    VkQueue handle;
};
class QueueFamilies
{
public:
    using index_t = std::uint32_t;
    static constexpr index_t INVALID_INDEX = std::numeric_limits<index_t>::max();
    struct Queue
    {
        VkQueue handle;
        index_t index;
    };
    explicit QueueFamilies(const PhysicalDevice& device, const Surface& surface);
public:
    [[nodiscard]] QueueView present() const;
    [[nodiscard]] QueueView graphics() const;
    [[nodiscard]] QueueView compute() const;
    [[nodiscard]] QueueView transfer() const;
    [[nodiscard]] std::vector<VkDeviceQueueCreateInfo> queue_create_info() const;
    [[nodiscard]] void store_queue_handles(const Device& device);
private:
    void select_queue_indices(const PhysicalDevice& device, const Surface& surface);
private:
    Queue m_Present;
    Queue m_Graphics;
    Queue m_Compute;
    Queue m_Transfer;
};
}    // namespace odin::graphics::vk
