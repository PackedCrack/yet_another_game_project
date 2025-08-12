#include "PhysicalDevice.hpp"
#include "vulkan_defines.hpp"
#include "vulkan_info.hpp"
//
//
namespace
{
struct GPU
{
    VkPhysicalDevice device;
    odin::graphics::vk::PhysicalDeviceProperties properties;
    odin::graphics::vk::PhysicalDeviceFeatures features;
    std::vector<VkQueueFamilyProperties> queueProperties;
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
[[nodiscard]] std::string& log_queue_properties(const GPU& gpu, std::string& msg)
{
    msg += std::format("\nFound {} queue families.", gpu.queueProperties.size());
    for (std::size_t i = 0; i < gpu.queueProperties.size(); ++i)
    {
        VkQueueFlags flags = gpu.queueProperties[i].queueFlags;
        msg += std::format("\n\tQueue Family {} supports:", i);
        if (flags & VK_QUEUE_GRAPHICS_BIT)
        {
            msg += "\n\t\tGraphics";
        }
        if (flags & VK_QUEUE_COMPUTE_BIT)
        {
            msg += "\n\t\tCompute";
        }
        if (flags & VK_QUEUE_TRANSFER_BIT)
        {
            msg += "\n\t\tTransfer";
        }
        if (flags & VK_QUEUE_SPARSE_BINDING_BIT)
        {
            msg += "\n\t\tSparse Binding";
        }
    }

    return msg;
}
[[nodiscard]] std::string& log_properties(const GPU& gpu, std::string& msg)
{
    switch (gpu.properties.device_type())
    {
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        msg += "\n\tDevice type: Other";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        msg += "\n\tDevice type: Integrated";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        msg += "\n\tDevice type: Discrete";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        msg += "\n\tDevice type: Virtual";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        msg += "\n\tDevice type: CPU";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
        LOG_FATAL("Unexpected Device Type: VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM.");
    }

    msg += std::format("\n\tDriver Name: {}", gpu.properties.driver_name());

    msg += std::format("\n\tDriver Version: {}", gpu.properties.driver_info());

    msg += std::format("\n\tVulkan API version: {}", gpu.properties.api_version());

    msg += std::format("\n\tMinimum Memory map alignment: {}", gpu.properties.min_memory_map_alignment());

    msg += std::format("\n\tMinimum Uniform buffer offset alignment: {}", gpu.properties.min_uniform_buffer_offset_alignment());

    msg += std::format("\n\tMinimum Storage buffer offset alignment: {}", gpu.properties.min_storage_buffer_offset_alignment());

    msg += std::format("\n\tMaximum Compute Work Group Total Invocations: {}", gpu.properties.max_compute_work_group_invocations());

    msg += std::format("\n\tMaximum Compute Work Group Size - X: {}", gpu.properties.work_group_size_x());

    msg += std::format("\n\tMaximum Compute Work Group Size - Y: {}", gpu.properties.work_group_size_y());

    msg += std::format("\n\tMaximum Compute Work Group Size - Z: {}", gpu.properties.work_group_size_z());

    return msg;
}
[[nodiscard]] std::string& log_features(const GPU& gpu, std::string& msg)
{
    msg += "\n\tSupports descriptor binding partially bound: ";
    msg += gpu.features.descriptor_binding_partially_bound() ? "True" : "False";

    msg += "\n\tSupports descriptor indexing: ";
    msg += gpu.features.descriptor_indexing() ? "True" : "False";

    msg += "\n\tSupports draw indirect count: ";
    msg += gpu.features.draw_indirect_count() ? "True" : "False";

    msg += "\n\tSupports Dynamic Rendering: ";
    msg += gpu.features.dynamic_rendering() ? "True" : "False";

    msg += "\n\tSupports Dynamic Rendering Local Read: ";
    msg += gpu.features.dynamic_rendering_local_read() ? "True" : "False";

    msg += "\n\tSupports Multi Draw Indirect: ";
    msg += gpu.features.multi_draw_indirect() ? "True" : "False";

    msg += "\n\tSupports Pipeline Statistic Queries: ";
    msg += gpu.features.pipeline_statistics_query() ? "True" : "False";

    msg += "\n\tSupports Runtime Descriptor Array: ";
    msg += gpu.features.runtime_descriptor_array() ? "True" : "False";

    msg += "\n\tSupports Shader Draw Parameters: ";
    msg += gpu.features.shader_draw_parameters() ? "True" : "False";

    msg += "\n\tSupports Synchronization 2: ";
    msg += gpu.features.synchronization2() ? "True" : "False";

    msg += "\n\tSupports Dynamic State 2: ";
    msg += gpu.features.supports_dynamic_state2() ? "True" : "False";

    msg += "\n\tSupports Dynamic State 3: ";
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
        msg = log_queue_properties(gpu, msg);

        LOG_INFO(msg);
    }
}
[[nodiscard]] bool supports_queue_requirements(const GPU& gpu, const odin::graphics::vk::Surface& surface)
{
    bool present{};
    bool graphics{};
    bool compute{};
    bool transfer{};
    std::uint32_t index{};
    for (auto&& property : gpu.queueProperties)
    {
        // Present support must call extension funcions and requires access to the surface..
        if (surface.queue_family_supports_present(gpu.device, index))
        {
            present = true;
        }
        if (property.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            graphics = true;
        }
        if (property.queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            compute = true;
        }
        if (property.queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            transfer = true;
        }
        // Spare binding is not a requirment as of now..
        //if (property.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
        //{
        //    sparseBinding = true;
        //}

        ++index;
    }

    return graphics && compute && transfer && present;
}
[[nodiscard]] std::vector<std::reference_wrapper<GPU>> find_discrete_gpus(std::vector<GPU>& gpus)
{
    std::vector<std::reference_wrapper<GPU>> discreteGpus{};
    auto action = [&discreteGpus](GPU& gpu)
    {
        if (gpu.properties.device_type() == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            discreteGpus.emplace_back(gpu);
        }
    };
    std::for_each(std::begin(gpus), std::end(gpus), action);

    return discreteGpus;
}
[[nodiscard]] GPU& select_gpu(std::vector<GPU>& gpus, const odin::graphics::vk::Surface& surface)
{
    ODIN_ASSERT(!gpus.empty());

    using Iterator = std::vector<GPU>::const_iterator;

    std::vector<std::reference_wrapper<GPU>> discreteGpus = find_discrete_gpus(gpus);
    if (discreteGpus.empty())
    {
        LOG_FATAL("No discrete GPU found! Integrated GPU's are not supported.");
    }

    std::vector<std::reference_wrapper<GPU>> alternatives{};
    for (auto&& gpu : discreteGpus)
    {
        const GPU& g = gpu.get();
        if (supports_queue_requirements(g, surface))
        {
            alternatives.emplace_back(g);
        }
    }
    if (alternatives.empty())
    {
        LOG_FATAL("No GPU found that supports queue requirements.");
    }

    // For now just return the first one..
    // May need a point system in the future - but most only have 1 dedicated gpu..
    return alternatives.front().get();
}
[[nodiscard]] std::vector<VkQueueFamilyProperties> queue_family_properties(VkPhysicalDevice device)
{
    std::uint32_t count = 0u;
    vkGetPhysicalDeviceQueueFamilyProperties(device, std::addressof(count), nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, queueFamilies.data());

    return queueFamilies;
}
[[nodiscard]] std::vector<VkPhysicalDevice> enumerate_physical_devices(const odin::graphics::vk::Instance& instance)
{
    std::uint32_t count{};
    VK_CHECK(vkEnumeratePhysicalDevices(instance.handle(), &count, nullptr), "Failed to enumerate physical device count.");
    if (count < 1u)
    {
        LOG_FATAL("No GPU found with Vulkan Support.");
    }

    std::vector<VkPhysicalDevice> devices(count);
    VK_CHECK(vkEnumeratePhysicalDevices(instance.handle(), &count, devices.data()), "Failed to enumerate physical device(s).");

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
        gpus.emplace_back(dev, Properties{ dev }, Features{ dev }, queue_family_properties(dev));
    }

    return gpus;
}
}    // namespace
namespace odin::graphics::vk
{
PhysicalDevice::PhysicalDevice(const Instance& instance, const Surface& surface)
    : m_PhysicalDevice{ VK_NULL_HANDLE }
    , m_Properties{}
    , m_Features{}
    , m_QueueProperties{}
//, m_Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 }
//, m_11Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES }
//, m_12Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES }
//, m_QueueFamilies{}
{
    std::vector<GPU> gpus = get_gpus(instance);
    GPU& selectedGPU = select_gpu(gpus, surface);
    log_gpus(gpus, selectedGPU);

    m_PhysicalDevice = selectedGPU.device;
    m_Properties = std::move(selectedGPU.properties);
    m_Features = std::move(selectedGPU.features);
    m_QueueProperties = std::move(selectedGPU.queueProperties);

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
    , m_QueueProperties{ other.m_QueueProperties }
{}
PhysicalDevice::PhysicalDevice(PhysicalDevice&& other) noexcept
    : m_PhysicalDevice{ std::exchange(other.m_PhysicalDevice, VK_NULL_HANDLE) }
    , m_Properties{ std::move(other.m_Properties) }
    , m_Features{ std::move(other.m_Features) }
    , m_QueueProperties{ std::move(other.m_QueueProperties) }
{}
PhysicalDevice& PhysicalDevice::operator=(const PhysicalDevice& other)
{
    if (this != std::addressof(other))
    {
        m_PhysicalDevice = other.m_PhysicalDevice;
        m_Properties = other.m_Properties;
        m_Features = other.m_Features;
        m_QueueProperties = other.m_QueueProperties;
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
        m_QueueProperties = std::move(other.m_QueueProperties);
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
const std::vector<VkQueueFamilyProperties>& PhysicalDevice::queue_families_properties() const
{
    return m_QueueProperties;
}
}    // namespace odin::graphics::vk
