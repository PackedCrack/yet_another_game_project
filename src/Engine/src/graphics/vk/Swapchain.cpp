#include "Swapchain.hpp"

#include "vulkan_defines.hpp"
#include "vulkan_info.hpp"
//
//
namespace
{
using namespace odin::graphics::vk;
[[nodiscard]] bool mode_exists(const std::vector<VkPresentModeKHR>& modes, VkPresentModeKHR mode)
{
    using Iterator = std::vector<VkPresentModeKHR>::const_iterator;
    Iterator it = std::find(std::begin(modes), std::end(modes), mode);
    return it != std::end(modes);
}
[[nodiscard]] VkPresentModeKHR select_present_mode(PhysicalDeviceRef physicalDevice, Surface& surface)
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
[[nodiscard]] std::uint32_t image_count(PhysicalDeviceRef physicalDevice, Surface& surface)
{
    std::uint32_t desired = 3;
    std::uint32_t max = surface.max_image_count(physicalDevice);
    // https://registry.khronos.org/vulkan/specs/latest/man/html/VkSurfaceCapabilitiesKHR.html
    //  A value of 0 means that there is no limit on the number of images,
    // though there may be limits related to the total amount of memory used by presentable images.
    max = max == 0 ? 3 : max;
    return std::min(desired, max);
}
[[nodiscard]] VkSurfaceFormatKHR select_surface_format(PhysicalDeviceRef physicalDevice, Surface& surface)
{
    using Iterator = std::vector<VkSurfaceFormatKHR>::const_iterator;
    std::vector<VkSurfaceFormatKHR> formats = surface.available_formats(physicalDevice);

    auto action = [](const VkSurfaceFormatKHR& format)
    { return format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; };
    Iterator it = std::find_if(std::begin(formats), std::end(formats), action);
    ODIN_ASSERT(it != std::end(formats), "Expecting this to exist for now..");

    return *it;
}
}    // namespace
namespace odin::graphics::vk
{
Swapchain::Swapchain(const Device& device, const PhysicalDevice& physicalDevice, Surface& surface)
    : m_Details{ make_details(physicalDevice.handle(), surface) }
    , m_Device{ device.handle() }
    , m_Swapchain{ create_swapchain(device.handle(), physicalDevice.handle(), surface, VK_NULL_HANDLE) }
    , m_Images{ swapchain_images(device.handle()) }
    , m_Views{}
{
    emplace_image_views();
}
Swapchain::Swapchain(DeviceRef device, PhysicalDeviceRef physicalDevice, Surface& surface, VkSwapchainKHR oldSwapchain)
    : m_Details{ make_details(physicalDevice, surface) }
    , m_Device{ device }
    , m_Swapchain{ create_swapchain(device, physicalDevice, surface, oldSwapchain) }
    , m_Images{ swapchain_images(device) }
    , m_Views{}
{
    emplace_image_views();
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
    , m_Swapchain{ VK_NULL_HANDLE }
    , m_Images{}
    , m_Views{}
{
    std::swap(m_Swapchain, other.m_Swapchain);
    std::swap(m_Images, other.m_Images);
    std::swap(m_Views, other.m_Views);
}
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
SwapchainRef Swapchain::handle() const
{
    ODIN_ASSERT(m_Swapchain != VK_NULL_HANDLE);
    return SwapchainRef{ .handle = m_Swapchain };
}
std::expected<Swapchain::AcquiredImage, Swapchain::Error> Swapchain::acquire(VkSemaphore imageAvailable) const
{
    static constexpr uint64_t timeout = std::numeric_limits<std::uint64_t>::max();

    Swapchain::index_t aquiredImage{};
    VkResult result =
        vkAcquireNextImageKHR(m_Device.handle, m_Swapchain, timeout, imageAvailable, VK_NULL_HANDLE, std::addressof(aquiredImage));
    if (result == VK_SUCCESS)
    {
        auto index = static_cast<std::size_t>(aquiredImage);
        return std::expected<AcquiredImage, Error>{
            std::in_place,
            AcquiredImage{ .image = resource::ImageRef{ .handle = m_Images[index] },
                          .view = m_Views[index].handle(),
                          .index = aquiredImage }
        };
    }
    else if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR)
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
[[nodiscard]] Swapchain::SwapchainDetails Swapchain::make_details(PhysicalDeviceRef physicalDevice, Surface& surface) const
{
    return SwapchainDetails{ .extent = surface.current_extent(physicalDevice),
                             .format = select_surface_format(physicalDevice, surface),
                             .presentMode = select_present_mode(physicalDevice, surface),
                             .imageCount = image_count(physicalDevice, surface) };
}
VkSwapchainKHR
Swapchain::create_swapchain(DeviceRef device, PhysicalDeviceRef physicalDevice, Surface& surface, VkSwapchainKHR oldSwapchain)
{
    // SHOULD BE VK_SHARING_MODE_CONCURRENT IF GRAPHICS AND PRESENT QUEUES ARE DIFFERENT
    SurfaceRef s = surface.handle();
    VkSwapchainCreateInfoKHR info = swapchain_create_info(s.handle,
                                                          m_Details.imageCount,
                                                          m_Details.presentMode,
                                                          m_Details.format.format,
                                                          m_Details.format.colorSpace,
                                                          m_Details.extent,
                                                          surface.current_transform(physicalDevice),
                                                          oldSwapchain);
    VK_CHECK(vkCreateSwapchainKHR(device.handle, std::addressof(info), nullptr, std::addressof(m_Swapchain)),
             "Failed to create Vulkan Swapchain.");

    return m_Swapchain;
}
std::vector<VkImage> Swapchain::swapchain_images(DeviceRef device)
{
    std::vector<VkImage> images(m_Details.imageCount);
    if (vkGetSwapchainImagesKHR(device.handle, m_Swapchain, std::addressof(m_Details.imageCount), images.data()) == VK_INCOMPLETE)
    {
        LOG_WARN("Did not recieve all images from the swapchain when calling GetSwapchainImages.");
    }

    return images;
}
void Swapchain::emplace_image_views()
{
    ODIN_ASSERT(!m_Images.empty());
    ODIN_ASSERT(m_Views.empty());

    VkFormat f = m_Details.format.format;
    VkImageViewCreateInfo info = image_view_create_info(VK_NULL_HANDLE, f, VK_IMAGE_ASPECT_COLOR_BIT);
    for (auto&& image : m_Images)
    {
        info.image = image;
        m_Views.emplace_back(m_Device, info);
    }
}
}    // namespace odin::graphics::vk
