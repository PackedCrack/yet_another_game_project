#include "Instance.hpp"


#include "vulkan_defines.hpp"
#include "vulkan_info.hpp"
namespace
{
[[nodiscard]] std::vector<VkExtensionProperties> enum_extension_properties()
{
    std::uint32_t count{};
    VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, std::addressof(count), nullptr),
             "Failed to obtain Instance Extension Properties count.");
    std::vector<VkExtensionProperties> availableExtensions(count);
    VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, std::addressof(count), availableExtensions.data()),
             "Failed to obtain Instance Extension Properties.");

    return availableExtensions;
}
[[nodiscard]] std::vector<VkLayerProperties> enum_layer_properties()
{
    std::uint32_t count{};
    VK_CHECK(vkEnumerateInstanceLayerProperties(std::addressof(count), nullptr), "Failed to obtain Instance Layer Properties count.");
    std::vector<VkLayerProperties> availableLayers(count);
    VK_CHECK(vkEnumerateInstanceLayerProperties(std::addressof(count), availableLayers.data()),
             "Failed to obtain Instance Layer Properties.");

    return availableLayers;
}
[[nodiscard]] std::vector<const char*> validation_layers()
{
#ifdef NDEBUG
    return std::vector<const char*>{};
#else
    // First get the available layers
    std::vector<VkLayerProperties> availableLayers = enum_layer_properties();

    // Just a quick and dirty check to make sure the validation layer acctually is availabe..
    constexpr std::string_view validationLayer = "VK_LAYER_KHRONOS_validation";
    bool foundLayer =
        std::any_of(std::begin(availableLayers),
                    std::end(availableLayers),
                    [&validationLayer](const VkLayerProperties& properties) { return (properties.layerName == validationLayer); });

    if (!foundLayer)
    {
        // For now just crash if validation layer is not availabe on this machine..
        LOG_FATAL("No validation layer found");
    }

    return std::vector<const char*>{ "VK_LAYER_KHRONOS_validation" };
#endif
}
#ifndef NDEBUG
[[nodiscard]] std::vector<const char*> required_extensions()
{
    return std::vector<const char*>{ VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME };
}
#else
[[nodiscard]] std::vector<const char*> required_extensions()
{
    return std::vector<const char*>{ VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME };
}
#endif
[[nodiscard]] std::vector<const char*> instance_extensions(const std::vector<std::string_view>& windowExtensions)
{
    // TODO: Do something with this
    [[maybe_unused]] std::vector<VkExtensionProperties> availableExtensions = enum_extension_properties();

    std::vector<const char*> extensions = required_extensions();
    std::transform(std::begin(windowExtensions),
                   std::end(windowExtensions),
                   std::back_inserter(extensions),
                   [](std::string_view sv) { return sv.data(); });

    return extensions;
}
}    // namespace
namespace odin::graphics::vk
{
Instance::Instance(VkApplicationInfo appInfo, const std::vector<std::string_view>& windowExtensions)
{
    const std::vector<const char*> layers = validation_layers();
    const std::vector<const char*> extensions = instance_extensions(windowExtensions);

    VkInstanceCreateInfo createInfo = instance_create_info(&appInfo, layers, extensions);
    VK_CHECK(vkCreateInstance(&createInfo, nullptr, &m_Instance), "Failed to create Vulkan Instance.");
}
Instance::~Instance()
{
    if (m_Instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(m_Instance, nullptr);
    }
}
Instance::Instance(Instance&& other) noexcept
    : m_Instance{ std::exchange(other.m_Instance, m_Instance) }
{}
Instance& Instance::operator=(Instance&& other) noexcept
{
    if (this != std::addressof(other))
    {
        std::swap(m_Instance, other.m_Instance);
    }

    return *this;
}
InstanceView Instance::view() const
{
    ODIN_ASSERT(m_Instance != VK_NULL_HANDLE);
    return InstanceView{ .handle = m_Instance };
}
}    // namespace odin::graphics::vk
