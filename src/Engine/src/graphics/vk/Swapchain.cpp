#include "Swapchain.hpp"

#include "vulkan_defines.hpp"
#include "vulkan_info.hpp"
//
//
namespace
{
[[nodiscard]] bool mode_exists(const std::vector<VkPresentModeKHR>& modes, VkPresentModeKHR mode)
{
    using Iterator = std::vector<VkPresentModeKHR>::const_iterator;
    Iterator it = std::find(std::begin(modes), std::end(modes), mode);
    return it != std::end(modes);
}
using namespace odin::graphics::vk;
[[nodiscard]] VkPresentModeKHR select_present_mode(const PhysicalDevice& physicalDevice, Surface& surface)
{
    std::vector<VkPresentModeKHR> modes = surface.present_modes(physicalDevice);
    if (mode_exists(modes, VK_PRESENT_MODE_MAILBOX_KHR))
    {
        return VK_PRESENT_MODE_MAILBOX_KHR;
    }
    else if (mode_exists(modes, VK_PRESENT_MODE_IMMEDIATE_KHR))
    {
        return VK_PRESENT_MODE_IMMEDIATE_KHR;
    }
    else if (mode_exists(modes, VK_PRESENT_MODE_FIFO_RELAXED_KHR))
    {
        return VK_PRESENT_MODE_FIFO_RELAXED_KHR;
    }

    ODIN_ASSERT(mode_exists(modes, VK_PRESENT_MODE_FIFO_KHR));
    return VK_PRESENT_MODE_FIFO_KHR;
}
[[nodiscard]] std::uint32_t image_count(const PhysicalDevice& physicalDevice, Surface& surface)
{
    std::uint32_t desired = 3;
    std::uint32_t max = surface.max_image_count(physicalDevice);
    return std::min(desired, max);
}
[[nodiscard]] VkSurfaceFormatKHR select_surface_format(const PhysicalDevice& physicalDevice, Surface& surface)
{
    using Iterator = std::vector<VkSurfaceFormatKHR>::const_iterator;
    std::vector<VkSurfaceFormatKHR> formats = surface.available_formats(physicalDevice);

    auto action = [](const VkSurfaceFormatKHR& format)
    { return format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; };
    Iterator it = std::find(std::begin(formats), std::end(formats), action);
    ODIN_ASSERT(it != std::end(formats), "Expecting this to exist for now..");

    return *it;
}
}    // namespace
namespace odin::graphics::vk
{
Swapchain::Swapchain(const Device& device, const PhysicalDevice& physicalDevice, Surface& surface, VkSwapchainKHR oldSwapchain)
    : m_Details{ make_details(physicalDevice, surface) }
    , m_Device{ device.handle() }
    , m_Swapchain{ create_swapchain(device, physicalDevice, surface, oldSwapchain) }
    , m_Images{ swapchain_images(device) }
    , m_Views{}
{
    ODIN_ASSERT(!m_Images.empty());
    ODIN_ASSERT(m_Views.empty());

    VkFormat f = m_Details.format.format;
    VkImageViewCreateInfo info = image_view_create_info(VK_NULL_HANDLE, f, VK_IMAGE_ASPECT_COLOR_BIT);
    for (auto&& image : m_Images)
    {
        info.image = image;
        m_Views.emplace_back(device, info);
    }
}
Swapchain::~Swapchain()
{
    if (m_Swapchain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(m_Device.handle, m_Swapchain, nullptr);
    }
}
Swapchain::Swapchain(Swapchain&& other) noexcept
    : m_Details{ other.m_Details }
    , m_Device{ other.m_Device }
    , m_Swapchain{ std::exchange(other.m_Swapchain, m_Swapchain) }
    , m_Images{ std::exchange(other.m_Images, m_Images) }
    , m_Views{ std::exchange(other.m_Views, m_Views) }
{}
Swapchain& Swapchain::operator=(Swapchain&& other) noexcept
{
    if (this != std::addressof(other))
    {
        m_Details = other.m_Details;
        m_Device = other.m_Device;
        m_Swapchain = std::exchange(other.m_Swapchain, m_Swapchain);
        m_Images = std::exchange(other.m_Images, m_Images);
        m_Views = std::exchange(other.m_Views, m_Views);
    }

    return *this;
}
//void Swapchain::rebuild(vulkan::Swapchain&& newSwapchain)
//{
//	// Swap all the resources that should be cleaned up from the old swapchain
//	std::swap(m_ImageCount, newSwapchain.m_ImageCount);
//	std::swap(m_ImageFormat, newSwapchain.m_ImageFormat);
//	std::swap(m_Images, newSwapchain.m_Images);
//	std::swap(m_ImageViews, newSwapchain.m_ImageViews);
//	std::swap(m_Extent, newSwapchain.m_Extent);
//	std::swap(m_pDevice, newSwapchain.m_pDevice);
//	std::swap(m_Swapchain, newSwapchain.m_Swapchain);
//}
VkSwapchainKHR Swapchain::handle() const
{
    ODIN_ASSERT(m_Swapchain != VK_NULL_HANDLE);

    return m_Swapchain;
}
std::expected<resource::ImageRef, Swapchain::Error> Swapchain::acquire(VkSemaphore renderer, uint32_t* pAquiredImage) const
{
    static constexpr uint64_t timeout = std::numeric_limits<std::uint64_t>::max();
    
    std::uint32_t aquiredImage{};
    VkResult result = vkAcquireNextImageKHR(m_Device.handle, m_Swapchain, timeout, renderer, VK_NULL_HANDLE, std::addressof(aquiredImage);
    if (result == VK_SUBOPTIMAL_KHR)
    {
        return std::unexpected{ Error::requiresRebuild };
    }
    else if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        return std::unexpected{ Error::requiresRebuild };
    }
    else if (result == VK_TIMEOUT)
    {
        return std::unexpected{ Error::timeout };
    }
    else
    {
        LOG_FATAL("Unexpected error when attempting to aquire image: {}", err_to_str(result));
    }

    auto index = static_cast<std::size_t>(aquiredImage);
    resource::ImageRef image{ .handle = m_Images[index] };
    return std::expected<resource::ImageRef, Error>{ std::in_place, image };
}
bool Swapchain::release(const QueueView& present, const std::vector<VkSemaphore>& renderer)
{
    // TODO: Maybe this should be in presenter?
    VkPresentInfoKHR info = { .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                              .pNext = nullptr,
                              .waitSemaphoreCount = static_cast<uint32_t>(renderer.size()),
                              .pWaitSemaphores = renderer.empty() ? nullptr : renderer.data(),
                              .swapchainCount = 1u,
                              .pSwapchains = std::addressof(m_Swapchain),
                              .pImageIndices = std::addressof(present.index),
                              .pResults = nullptr };

    if (VkResult result = vkQueuePresentKHR(present.handle, std::addressof(info)); result != VK_SUCCESS)
    {
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            return false;
        }
        LOG_FATAL("Unexpected error when attempting to present image: {}", err_to_str(result));
    }

    return true;
}
VkFormat Swapchain::color_format() const
{
    return m_Details.format.format;
}
const std::vector<resource::ImageView>& Swapchain::image_views() const
{
    ODIN_ASSERT(!m_Views.empty());
    return m_Views;
}
const VkExtent2D& Swapchain::extent() const
{
    return m_Details.extent;
}
[[nodiscard]] Swapchain::SwapchainDetails Swapchain::make_details(const PhysicalDevice& physicalDevice, Surface& surface) const
{
    return SwapchainDetails{ .extent = surface.current_extent(physicalDevice),
                             .format = select_surface_format(physicalDevice, surface),
                             .presentMode = select_present_mode(physicalDevice, surface),
                             .imageCount = image_count(physicalDevice, surface) };
}
VkSwapchainKHR
Swapchain::create_swapchain(const Device& device, const PhysicalDevice& physicalDevice, Surface& surface, VkSwapchainKHR oldSwapchain)
{
    SurfaceRef s = surface.handle();
    DeviceRef d = device.handle();

    VkSwapchainCreateInfoKHR info = swapchain_create_info(s.handle,
                                                          m_Details.imageCount,
                                                          m_Details.presentMode,
                                                          m_Details.format.format,
                                                          m_Details.format.colorSpace,
                                                          m_Details.extent,
                                                          surface.current_transform(physicalDevice),
                                                          oldSwapchain);
    VK_CHECK(vkCreateSwapchainKHR(d.handle, std::addressof(info), nullptr, std::addressof(m_Swapchain)),
             "Failed to create Vulkan Swapchain.");

    return m_Swapchain;
}
std::vector<VkImage> Swapchain::swapchain_images(const Device& device)
{
    DeviceRef d = device.handle();
    std::vector<VkImage> images(m_Details.imageCount);
    if (vkGetSwapchainImagesKHR(d.handle, m_Swapchain, std::addressof(m_Details.imageCount), images.data()) == VK_INCOMPLETE)
    {
        LOG_WARN("Did not recieve all images from the swapchain when calling GetSwapchainImages.");
    }

    return images;
}
}    // namespace odin::graphics::vk
