#pragma once

#include "Device.hpp"
#include "PhysicalDevice.hpp"
#include "Surface.hpp"
#include "QueueFamilies.hpp"
#include "resource/Image.hpp"
#include "resource/ImageView.hpp"
// std
#include <expected>    // pch?
//
//
namespace odin::graphics::vk
{
struct SwapchainRef
{
    VkSwapchainKHR handle;
};
class Swapchain
{
    struct SwapchainDetails
    {
        VkExtent2D extent;
        VkSurfaceFormatKHR format;
        VkPresentModeKHR presentMode;
        std::uint32_t imageCount;
    };
public:
    using index_t = std::uint32_t;
    struct AcquiredImage
    {
        resource::ImageRef image;
        resource::ImageViewRef view;
        index_t index;
    };
    enum class Error
    {
        requiresRebuild,
        timeout
    };
public:
    Swapchain(const Device& device, const PhysicalDevice& physicalDevice, Surface& surface);
    Swapchain(DeviceRef device, PhysicalDeviceRef physicalDevice, Surface& surface, VkSwapchainKHR oldSwapchain);
    ~Swapchain();
    Swapchain(const Swapchain& other) = delete;
    Swapchain(Swapchain&& other) noexcept;
    Swapchain& operator=(const Swapchain& other) = delete;
    Swapchain& operator=(Swapchain&& other) noexcept;
public:
    [[nodiscard]] SwapchainRef handle() const;
    [[nodiscard]] std::expected<AcquiredImage, Error> acquire(VkSemaphore imageAvailable) const;
    [[nodiscard]] VkFormat color_format() const;
    [[nodiscard]] const std::vector<resource::ImageView>& image_views() const;
    [[nodiscard]] const VkExtent2D& extent() const;
private:
    [[nodiscard]] SwapchainDetails make_details(PhysicalDeviceRef physicalDevice, Surface& surface) const;
    [[nodiscard]] VkSwapchainKHR
    create_swapchain(DeviceRef device, PhysicalDeviceRef physicalDevice, Surface& surface, VkSwapchainKHR oldSwapchain);
    [[nodiscard]] std::vector<VkImage> swapchain_images(DeviceRef device);
    void emplace_image_views();
private:
    SwapchainDetails m_Details;
    DeviceRef m_Device;
    VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_Images;
    std::vector<resource::ImageView> m_Views;
};
}    // namespace odin::graphics::vk
