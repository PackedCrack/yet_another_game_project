#pragma once

#include "Device.hpp"
#include "PhysicalDevice.hpp"
#include "Surface.hpp"
#include "QueueFamilies.hpp"
#include "resource/Image.hpp"
#include "resource/ImageView.hpp"
// std
#include <expected> // pch?
//
//
namespace odin::graphics::vk
{
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
    enum class Error
    {
        requiresRebuild,
        timeout
    };
public: 
    Swapchain(const Device& device, const PhysicalDevice& physicalDevice, Surface& surface, VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE);
    ~Swapchain();
    Swapchain(const Swapchain& other) = delete;
    Swapchain(Swapchain&& other) noexcept;
    Swapchain& operator=(const Swapchain& other) = delete;
    Swapchain& operator=(Swapchain&& other) noexcept;
public:
    //void rebuild(Swapchain&& newSwapchain);
    [[nodiscard]] VkSwapchainKHR handle() const;
    [[nodiscard]] std::expected<resource::ImageRef, Error> acquire(VkSemaphore renderer, uint32_t* pAquiredImage) const;
    // Should be in Presenter?
    [[nodiscard]] bool release(const QueueView& present, const std::vector<VkSemaphore>& renderer);
    [[nodiscard]] VkFormat color_format() const;
    [[nodiscard]] const std::vector<resource::ImageView>& image_views() const;
    [[nodiscard]] const VkExtent2D& extent() const;
private:
    [[nodiscard]] SwapchainDetails make_details(const PhysicalDevice& physicalDevice, Surface& surface) const;
    [[nodiscard]] VkSwapchainKHR
    create_swapchain(const Device& device, const PhysicalDevice& physicalDevice, Surface& surface, VkSwapchainKHR oldSwapchain);
    [[nodiscard]] std::vector<VkImage> swapchain_images(const Device& device);
private:
    SwapchainDetails m_Details;
    DeviceRef m_Device;
    VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_Images;
    std::vector<resource::ImageView> m_Views;
};
}    // namespace odin::graphics::vk
