#include "PhysicalDevice.hpp"
#include "vulkan_defines.hpp"
#include "vulkan_info.hpp"
namespace
{
struct GPU
{
    VkPhysicalDevice device;
    odin::graphics::vk::PhysicalDeviceProperties properties;
    odin::graphics::vk::PhysicalDeviceFeatures features;
};
//[[nodiscard]] std::string api_version_to_str(uint32_t apiVersion)
//{
//    std::string version = std::to_string(VK_API_VERSION_MAJOR(apiVersion));
//    version += ".";
//    version += std::to_string(VK_API_VERSION_MINOR(apiVersion));
//    version += ".";
//    version += std::to_string(VK_API_VERSION_PATCH(apiVersion));
//    return version;
//}
//[[nodiscard]] uint32_t nvidia_major_version(uint32_t driverVersion)
//{
//    return (driverVersion >> 22u) & 0x3'ff;
//}
//[[nodiscard]] uint32_t nvidia_minor_version(uint32_t driverVersion)
//{
//    return (driverVersion >> 14u) & 0x0'ff;
//}
//[[nodiscard]] uint32_t nvidia_patch_version(uint32_t driverVersion)
//{
//    return (driverVersion >> 6u) & 0x0'ff;
//}
//[[nodiscard]] uint32_t nvidia_dick_version(uint32_t driverVersion)
//{
//    return driverVersion & 0x00'3f;
//}
//[[nodiscard]] std::string nvidia_driver_to_str(uint32_t driverVersion)
//{
//    std::string driver = std::to_string(nvidia_major_version(driverVersion));
//    driver += ".";
//    driver += std::to_string(nvidia_minor_version(driverVersion));
//    driver += ".";
//    driver += std::to_string(nvidia_patch_version(driverVersion));
//    driver += ".";
//    driver += std::to_string(nvidia_dick_version(driverVersion));
//    return driver;
//}

//void log_queue_family_properties(const std::vector<VkQueueFamilyProperties>& properties)
//{
//    std::string logMsg("Found " + std::to_string(properties.size()) + "'s queue families.");
//    for (uint32_t i = 0; i < properties.size(); ++i)
//    {
//        logMsg += "\n\tAvailable Queue Family " + std::to_string(i) + " supports:";
//        if (properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
//        {
//            logMsg += "\nGraphics";
//        }
//        if (properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
//        {
//            logMsg += "\nCompute";
//        }
//        if (properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
//        {
//            logMsg += "\nTransfer";
//        }
//        if (properties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
//        {
//            logMsg += "\nSparse Binding";
//        }
//    }
//
//    logger::info(logMsg);
//}
//void log_queue_families_selection(odin::vulkan::QueueFamilyIndices& queueIndices)
//{
//    std::string message = "Queue Families Selection: ";
//    if (queueIndices.graphics.has_value())
//    {
//        message += "\nGraphics: " + std::to_string(queueIndices.graphics.value());
//    }
//    else
//    {
//        ODIN_ERROR_SEVERE("FATAL::NO::GRAPHICS::QUEUE::SELECTED");
//    }
//
//    if (queueIndices.present.has_value())
//    {
//        message += "\nPresent: " + std::to_string(queueIndices.present.value());
//    }
//    else
//    {
//        ODIN_ERROR_SEVERE("FATAL::NO::PRESENT::QUEUE::SELECTED");
//    }
//
//    if (queueIndices.transfer.has_value())
//    {
//        message += "\nTransfer: " + std::to_string(queueIndices.transfer.value());
//    }
//    else
//    {
//        ODIN_ERROR_SEVERE("FATAL::NO::TRANSFER::QUEUE::SELECTED");
//    }
//
//    if (queueIndices.compute.has_value())
//    {
//        message += "\nCompute: " + std::to_string(queueIndices.compute.value());
//    }
//    else
//    {
//        message += "\nNo compute queue selected.";
//    }
//
//    logger::info(message);
//}
//[[nodiscard]] std::vector<GPUProperties> enumerate_properties(const std::vector<VkPhysicalDevice>& devices)
//{
//    std::vector<GPUProperties> gpuProperties{};
//    gpuProperties.resize(devices.size());
//
//    for (size_t i = 0u; i < devices.size(); ++i)
//    {
//        GPUProperties& gpu = gpuProperties[i];
//        gpu.device = devices[i];
//        gpu.properties12 = odin::vkstruct::physical_device_properies_vulkan_12();
//        gpu.properties11 = odin::vkstruct::physical_device_properies_vulkan_11(&gpu.properties12);
//        gpu.properties = odin::vkstruct::physical_device_properies_2(&gpu.properties11);
//
//        vkGetPhysicalDeviceProperties2(gpu.device, &gpu.properties);
//    }
//
//    return gpuProperties;
//}

//[[nodiscard]] odin::vulkan::QueueFamilyIndices query_queue_families(VkPhysicalDevice gpu, VkSurfaceKHR surface)
//{
//    uint32_t queueCount = 0u;
//    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueCount, nullptr);
//    if (queueCount < 1u)
//    {
//        ODIN_ERR("FATAL::FOUND::NO::QUEUE::FAMILIES");
//    }
//
//    std::vector<VkQueueFamilyProperties> queueFamilies(queueCount);
//    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueCount, queueFamilies.data());
//
//#ifndef NDEBUG
//    log_queue_family_properties(queueFamilies);
//#endif
//
//    uint32_t index = 0u;
//    odin::vulkan::QueueFamilyIndices queueIndices = {};
//    for (const auto& queueProperties : queueFamilies)
//    {
//        // TODO:: fix if we want to utilize several queues
//        VkBool32 surfaceSupport = false;
//        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu, index, surface, &surfaceSupport));
//        const VkQueueFlags QUEUE_FLAGS = queueProperties.queueFlags;
//
//        bool supportGraphics = QUEUE_FLAGS & VK_QUEUE_GRAPHICS_BIT;
//        bool supportCompute = QUEUE_FLAGS & VK_QUEUE_COMPUTE_BIT;
//        bool supportTransfer = QUEUE_FLAGS & VK_QUEUE_TRANSFER_BIT;
//
//        if (supportGraphics && surfaceSupport)
//        {
//            queueIndices.graphics = index;
//            queueIndices.present = index;
//        }
//
//        if (supportTransfer && !supportCompute)
//        {
//            queueIndices.transfer = index;
//        }
//
//        if (supportCompute && index != queueIndices.transfer)
//        {
//            queueIndices.compute = index;
//        }
//
//        ++index;
//    }
//
//
//    // TODO:: MULTIPLE QUEUE NOT IMPLELEMENTED
//    queueIndices.graphics = 0;
//    queueIndices.present = 0;
//    queueIndices.transfer = 0;
//    queueIndices.compute = 0;
//
//
//    // TODO:: improve queue selection logic
//    if (!queueIndices.graphics.has_value())
//    {
//        ODIN_ERROR_SEVERE("FATAL::NO::GRAPHIC::QUEUE::FAMILY::SELECTED");
//    }
//    if (!queueIndices.present.has_value())
//    {
//        ODIN_ERROR_SEVERE("FATAL::NO::SURFACE::QUEUE::FAMILY::SELECTED");
//    }
//    if (!queueIndices.compute.has_value())
//    {
//        ODIN_ERROR_SEVERE("FATAL::NO::COMPUTE::QUEUE::FAMILY::SELECTED");
//    }
//    if (!queueIndices.transfer.has_value())
//    {
//        ODIN_ERROR_SEVERE("FATAL::NO::TRANSFER::QUEUE::FAMILY::SELECTED");
//    }
//
//#ifndef NDEBUG
//    log_queue_families_selection(queueIndices);
//#endif
//
//    return queueIndices;
//}
[[nodiscard]] std::string& log_properties(const GPU& gpu, std::string& msg)
{
    switch (gpu.properties.device_type())
    {
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        msg += "\nDevice type: Other";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        msg += "\nDevice type: Integrated";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        msg += "\nDevice type: Discrete";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        msg += "\nDevice type: Virtual";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        msg += "\nDevice type: CPU";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
        LOG_FATAL("Unexpected Device Type: VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM.");
    }

    msg += "\nDriver Name: ";
    msg += gpu.properties.driver_name();

    msg += "\nDriver Version: ";
    msg += gpu.properties.driver_info(); 
    
    msg += "\nVulkan API version: ";
    msg += gpu.properties.api_version();
    
    msg += "\nMinimum Memory map alignment: ";
    msg += std::to_string(gpu.properties.min_memory_map_alignment());
    
    msg += "\nMinimum Uniform buffer offset alignment: ";
    msg += std::to_string(gpu.properties.min_uniform_buffer_offset_alignment());
    
    msg += "\nMinimum Storage buffer offset alignment: ";
    msg += std::to_string(gpu.properties.min_storage_buffer_offset_alignment());
    
    msg += "\nMaximum Compute Work Group Total Invocations: ";
    msg += std::to_string(gpu.properties.max_compute_work_group_invocations());
    
    msg += "\nMaximum Compute Work Group Size - X: ";
    msg += std::to_string(gpu.properties.work_group_size_x());
    
    msg += "\nMaximum Compute Work Group Size - Y: ";
    msg += std::to_string(gpu.properties.work_group_size_y());
    
    msg += "\nMaximum Compute Work Group Size - Z: ";
    msg += std::to_string(gpu.properties.work_group_size_z());

    return msg;
}
[[nodiscard]] std::string& log_features(const GPU& gpu, std::string& msg)
{
    msg += "\nSupports descriptor binding partially bound: ";
    msg += gpu.features.descriptor_binding_partially_bound() ? "True" : "False";

    msg += "\nSupports descriptor indexing: ";
    msg += gpu.features.descriptor_indexing() ? "True" : "False";

    msg += "\nSupports draw indirect count: ";
    msg += gpu.features.draw_indirect_count() ? "True" : "False";

    msg += "\nSupports Dynamic Rendering: ";
    msg += gpu.features.dynamic_rendering() ? "True" : "False";

    msg += "\nSupports Dynamic Rendering Local Read: ";
    msg += gpu.features.dynamic_rendering_local_read() ? "True" : "False";

    msg += "\nSupports Multi Draw Indirect: ";
    msg += gpu.features.multi_draw_indirect() ? "True" : "False";

    msg += "\nSupports Pipeline Statistic Queries: ";
    msg += gpu.features.pipeline_statistics_query() ? "True" : "False";

    msg += "\nSupports Runtime Descriptor Array: ";
    msg += gpu.features.runtime_descriptor_array() ? "True" : "False";

    msg += "\nSupports Shader Draw Parameters: ";
    msg += gpu.features.shader_draw_parameters() ? "True" : "False";

    msg += "\nSupports Synchronization 2: ";
    msg += gpu.features.synchronization2() ? "True" : "False";

    msg += "\nSupports Dynamic State 2: ";
    msg += gpu.features.supports_dynamic_state2() ? "True" : "False";

    msg += "\nSupports Dynamic State 3: ";
    msg += gpu.features.supports_dynamic_state3() ? "True" : "False";

    return msg;
}
void log_gpus(const std::vector<GPU>& gpus, const GPU& selected)
{
    for (auto&& gpu : gpus)
    {
        std::string msg{ "GPU: " };
        msg += gpu.properties.device_name();
        msg += (gpu.device == selected.device) ? " (Selected) " : "";

        msg = log_properties(gpu, msg);
        msg = log_features(gpu, msg);

        LOG_INFO(msg);
    }
}
[[nodiscard]] std::vector<VkPhysicalDevice> enumerate_physical_devices(const odin::graphics::vk::Instance& instance)
{
    std::uint32_t count{};
    VK_CHECK(vkEnumeratePhysicalDevices(instance.handle(), &count, nullptr), 
             "Failed to enumerate physical device count.");
    if (count < 1u)
    {
        LOG_FATAL("No GPU found with Vulkan Support.");
    }

    std::vector<VkPhysicalDevice> devices(count);
    VK_CHECK(vkEnumeratePhysicalDevices(instance.handle(), &count, devices.data()),
        "Failed to enumerate physical device(s).");

    return devices;
}
[[nodiscard]] std::vector<GPU> get_gpus(const odin::graphics::vk::Instance& instance)
{
    using Properties = odin::graphics::vk::PhysicalDeviceProperties;
    using Features = odin::graphics::vk::PhysicalDeviceFeatures;

    std::vector<VkPhysicalDevice> physicalDevices = enumerate_physical_devices(instance);

    std::vector<GPU> gpus{};
    for (VkPhysicalDevice dev : physicalDevices)
    {
        gpus.emplace_back(dev, Properties{ dev }, Features{ dev });
    }

    return gpus;
}
[[nodiscard]] GPU select_gpu(const std::vector<GPU>& gpus)
{
    ODIN_ASSERT(!gpus.empty());

    auto it =
        std::find_if(std::begin(gpus),
                     std::end(gpus),
                     [] (const GPU& gpu) { return gpu.properties.device_type() == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU; });

    if (it == std::end(gpus))
    {
        LOG_FATAL("No discrete GPU found! Integrated GPU's are not supported.");
    }

    return *it;
}
}    // namespace
namespace odin::graphics::vk
{
PhysicalDevice::PhysicalDevice(const Instance& instance)
    : m_PhysicalDevice{ VK_NULL_HANDLE }
    , m_Properties{}
    , m_Features{}
    //, m_Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 }
    //, m_11Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES }
    //, m_12Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES }
    //, m_QueueFamilies{}
{
    std::vector<GPU> gpus = get_gpus(instance);
    GPU selectedGPU = select_gpu(gpus);
    log_gpus(gpus, selectedGPU);

    m_PhysicalDevice = selectedGPU.device;
    m_Properties = std::move(selectedGPU.properties);
    m_Features = std::move(selectedGPU.features);

//    // TODO:: Acctual gpu selection logic
//    auto deviceAndProperties = enumerate_properties(availableDevices);
//    auto selectedGPU = select_gpu(deviceAndProperties);
//    m_PhysicalDevice = selectedGPU.device;
//    m_Properties = std::move(selectedGPU.properties);
//    m_11Properties = std::move(selectedGPU.properties11);
//    m_12Properties = std::move(selectedGPU.properties12);
//    m_Properties.pNext = &m_11Properties;
//    m_11Properties.pNext = &m_12Properties;
//
//
//#ifndef NDEBUG
//    logger::info("Found " + std::to_string(availableDevices.size()) + " GPU(s)");
//    for (const auto& gpu : deviceAndProperties)
//    {
//        std::string message{};
//        // Log device properties
//        log_physical_device_propertes(gpu, m_PhysicalDevice, message);
//        // Log device features
//        log_physical_device_features(gpu.device, message);
//
//        logger::info(message);
//    }
//#endif


    // Get features
    //m_11Features.pNext = &m_12Features;
    //m_Features.pNext = &m_11Features;
    //vkGetPhysicalDeviceFeatures2(m_PhysicalDevice, &m_Features);
    //
    //m_QueueFamilies = query_queue_families(m_PhysicalDevice, window.surface());
    //
    //window.create_surface_format(*this);
}
PhysicalDevice::PhysicalDevice(const PhysicalDevice& other)
    : m_PhysicalDevice{ other.m_PhysicalDevice }
    , m_Properties{ other.m_Properties }
    , m_Features{ other.m_Features }
{}
PhysicalDevice::PhysicalDevice(PhysicalDevice&& other) noexcept
    : m_PhysicalDevice{ std::exchange(other.m_PhysicalDevice, VK_NULL_HANDLE) }
    , m_Properties{ std::move(other.m_Properties) }
    , m_Features{ std::move(other.m_Features) }
{}
PhysicalDevice& PhysicalDevice::operator=(const PhysicalDevice& other)
{
    if (this != std::addressof(other))
    {
        m_PhysicalDevice = other.m_PhysicalDevice;
        m_Properties = other.m_Properties;
        m_Features = other.m_Features;
    }
    
    return *this;
}
PhysicalDevice& PhysicalDevice::operator=(PhysicalDevice&& other) noexcept
{
    if (this != std::addressof(other))
    {
        m_PhysicalDevice = std::exchange(other.m_PhysicalDevice, VK_NULL_HANDLE);
        m_Properties = std::move(other.m_Properties);
        m_Features = std::move(other.m_Features);
    }

    return *this;
}
VkPhysicalDevice PhysicalDevice::handle() const
{
    ODIN_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE);

    return m_PhysicalDevice;
}
//uint32_t PhysicalDevice::queue_index_graphics() const
//{
//    ODIN_ASSERT(m_QueueFamilies.graphics.has_value());    // TODO:: FIX LATER WILL ONLY USE GRAPHICS/PRESENT NOW
//
//    return m_QueueFamilies.graphics.value();
//}
//uint32_t PhysicalDevice::queue_index_present() const
//{
//    ODIN_ASSERT(m_QueueFamilies.present.has_value());    // TODO:: FIX LATER WILL ONLY USE GRAPHICS/PRESENT NOW
//
//    return m_QueueFamilies.present.value();
//}
//uint32_t PhysicalDevice::queue_index_compute() const
//{
//    ODIN_ASSERT(m_QueueFamilies.compute.has_value());    // TODO:: FIX LATER WILL ONLY USE GRAPHICS/PRESENT NOW
//
//    return m_QueueFamilies.compute.value();
//}
//uint32_t PhysicalDevice::queue_index_transfer() const
//{
//    ODIN_ASSERT(m_QueueFamilies.transfer.has_value());    // TODO:: FIX LATER WILL ONLY USE GRAPHICS/PRESENT NOW
//
//    return m_QueueFamilies.transfer.value();
//}
const PhysicalDeviceProperties& PhysicalDevice::properties() const
{
    return m_Properties;
}
const PhysicalDeviceFeatures& PhysicalDevice::features() const
{
    return m_Features;
}
}    // namespace odin::graphics::vk
