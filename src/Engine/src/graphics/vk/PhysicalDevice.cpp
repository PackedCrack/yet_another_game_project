//
// Created by qwerty on 11/08/2025.
//
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
[[nodiscard]] std::string log_queue_properties(const GPU& gpu, const odin::graphics::vk::Surface& surface)
{
    std::string msg = std::format("\n\tFound {} queue families:", gpu.queueProperties.size());
    for (std::size_t i = 0; i < gpu.queueProperties.size(); ++i)
    {
        VkQueueFlags flags = gpu.queueProperties[i].queueFlags;
        msg += std::format("\n\t\tQueue Family {} supports:", i);
        if (surface.queue_family_supports_present(gpu.device, static_cast<std::uint32_t>(i)))
        {
            msg += "\n\t\t\tPresent";
        }
        if (flags & VK_QUEUE_GRAPHICS_BIT)
        {
            msg += "\n\t\t\tGraphics";
        }
        if (flags & VK_QUEUE_COMPUTE_BIT)
        {
            msg += "\n\t\t\tCompute";
        }
        if (flags & VK_QUEUE_TRANSFER_BIT)
        {
            msg += "\n\t\t\tTransfer";
        }
        if (flags & VK_QUEUE_SPARSE_BINDING_BIT)
        {
            msg += "\n\t\t\tSparse Binding";
        }
    }

    return msg;
}
[[nodiscard]] std::string log_properties(const GPU& gpu)
{
    std::string msg{ "\n\tProperties: " };
    switch (gpu.properties.device_type())
    {
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        msg += "\n\t\tDevice type: Other";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        msg += "\n\t\tDevice type: Integrated";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        msg += "\n\t\tDevice type: Discrete";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        msg += "\n\t\tDevice type: Virtual";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        msg += "\n\t\tDevice type: CPU";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
        LOG_FATAL("Unexpected Device Type: VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM.");
    }

    msg += std::format("\n\t\tDriver Name: {}", gpu.properties.driver_name());

    msg += std::format("\n\t\tDriver Version: {}", gpu.properties.driver_info());

    msg += std::format("\n\t\tVulkan API version: {}", gpu.properties.api_version());

    msg += std::format("\n\t\tMinimum Memory map alignment: {}", gpu.properties.min_memory_map_alignment());

    msg += std::format("\n\t\tMinimum Uniform buffer offset alignment: {}", gpu.properties.min_uniform_buffer_offset_alignment());

    msg += std::format("\n\t\tMinimum Storage buffer offset alignment: {}", gpu.properties.min_storage_buffer_offset_alignment());

    msg += std::format("\n\t\tMaximum Compute Work Group Total Invocations: {}", gpu.properties.max_compute_work_group_invocations());

    msg += std::format("\n\t\tMaximum Compute Work Group Size - X: {}", gpu.properties.work_group_size_x());

    msg += std::format("\n\t\tMaximum Compute Work Group Size - Y: {}", gpu.properties.work_group_size_y());

    msg += std::format("\n\t\tMaximum Compute Work Group Size - Z: {}", gpu.properties.work_group_size_z());

    return msg;
}
[[nodiscard]] std::string log_feature_support(VkBool32 support)
{
    return support ? "Yes" : "No";
}
[[nodiscard]] std::string log_features(const GPU& gpu)
{
    std::string msg = "\n\tFeature Support:";

    msg += "\n\t\tSupports descriptor binding partially bound: ";
    msg += log_feature_support(gpu.features.descriptor_binding_partially_bound());

    msg += "\n\t\tSupports descriptor indexing: ";
    msg += log_feature_support(gpu.features.descriptor_indexing());

    msg += "\n\t\tSupports draw indirect count: ";
    msg += log_feature_support(gpu.features.draw_indirect_count());

    msg += "\n\t\tSupports Dynamic Rendering: ";
    msg += log_feature_support(gpu.features.dynamic_rendering());

    msg += "\n\t\tSupports Dynamic Rendering Local Read: ";
    msg += log_feature_support(gpu.features.dynamic_rendering_local_read());

    msg += "\n\t\tSupports Multi Draw Indirect: ";
    msg += log_feature_support(gpu.features.multi_draw_indirect());

    msg += "\n\t\tSupports Pipeline Statistic Queries: ";
    msg += log_feature_support(gpu.features.pipeline_statistics_query());

    msg += "\n\t\tSupports Runtime Descriptor Array: ";
    msg += log_feature_support(gpu.features.runtime_descriptor_array());

    msg += "\n\t\tSupports Shader Draw Parameters: ";
    msg += log_feature_support(gpu.features.shader_draw_parameters());

    msg += "\n\t\tSupports Synchronization 2: ";
    msg += log_feature_support(gpu.features.synchronization2());

    msg += "\n\t\tSupports Dynamic State 2: ";
    msg += log_feature_support(gpu.features.supports_dynamic_state2());

    msg += "\n\t\tSupports Dynamic State 3: ";
    msg += log_feature_support(gpu.features.supports_dynamic_state3());

    return msg;
}
void log_gpus(const std::vector<GPU>& gpus, const GPU& selected, const odin::graphics::vk::Surface& surface)
{
    for (std::size_t i = 0; i < gpus.size(); ++i)
    {
        const GPU& gpu = gpus[i];

        std::string msg = std::format("GPU Details ({} of {}):\n", i + 1, gpus.size());
        msg += gpu.properties.device_name();
        msg += (gpu.device == selected.device) ? " (Selected) " : "";

        msg += log_properties(gpu);
        msg += log_features(gpu);
        msg += log_queue_properties(gpu, surface);

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
[[nodiscard]] std::reference_wrapper<GPU> select_gpu(std::vector<GPU>& gpus, const odin::graphics::vk::Surface& surface)
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
        GPU& g = gpu.get();
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
    return alternatives.front();
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
    odin::graphics::vk::InstanceRef i = instance.handle();
    VK_CHECK(vkEnumeratePhysicalDevices(i.handle, &count, nullptr), "Failed to enumerate physical device count.");
    if (count < 1u)
    {
        LOG_FATAL("No GPU found with Vulkan Support.");
    }

    std::vector<VkPhysicalDevice> devices(count);
    VK_CHECK(vkEnumeratePhysicalDevices(i.handle, &count, devices.data()), "Failed to enumerate physical device(s).");

    return devices;
}
[[nodiscard]] std::vector<GPU> get_gpus(const odin::graphics::vk::Instance& instance)
{
    using Properties = odin::graphics::vk::PhysicalDeviceProperties;
    using Features = odin::graphics::vk::PhysicalDeviceFeatures;

    std::vector<VkPhysicalDevice> physicalDevices = enumerate_physical_devices(instance);

    std::vector<GPU> gpus{};
    auto t = [](VkPhysicalDevice dev)
    {
        return GPU{ .device = dev,
                    .properties = Properties{ dev },
                    .features = Features{ dev },
                    .queueProperties = queue_family_properties(dev) };
    };
    std::transform(std::begin(physicalDevices), std::end(physicalDevices), std::back_inserter(gpus), t);

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
{
    std::vector<GPU> gpus = get_gpus(instance);
    std::reference_wrapper<GPU> selectedGPU = select_gpu(gpus, surface);
    GPU& selected = selectedGPU.get();

    log_gpus(gpus, selected, surface);

    m_PhysicalDevice = selected.device;
    m_Properties = std::move(selected.properties);
    m_Features = std::move(selected.features);
    m_QueueProperties = std::move(selected.queueProperties);
}
PhysicalDevice::PhysicalDevice(const PhysicalDevice& other)
    : m_PhysicalDevice{ other.m_PhysicalDevice }
    , m_Properties{ other.m_Properties }
    , m_Features{ other.m_Features }
    , m_QueueProperties{ other.m_QueueProperties }
{}
PhysicalDevice::PhysicalDevice(PhysicalDevice&& other) noexcept
    : m_PhysicalDevice{ VK_NULL_HANDLE }
    , m_Properties{ std::move(other.m_Properties) }
    , m_Features{ std::move(other.m_Features) }
    , m_QueueProperties{ std::move(other.m_QueueProperties) }
{
    std::swap(m_PhysicalDevice, other.m_PhysicalDevice);
}
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
        m_PhysicalDevice = std::exchange(other.m_PhysicalDevice, m_PhysicalDevice);
        m_Properties = std::move(other.m_Properties);
        m_Features = std::move(other.m_Features);
        m_QueueProperties = std::move(other.m_QueueProperties);
    }

    return *this;
}
PhysicalDeviceRef PhysicalDevice::handle() const
{
    ODIN_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE);

    return PhysicalDeviceRef{ .handle = m_PhysicalDevice };
}
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
