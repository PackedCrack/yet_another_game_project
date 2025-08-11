#pragma once
//
//
#include <vulkan/vulkan.h>
//
namespace odin::graphics::vk
{
constexpr VkApplicationInfo application_info(const char* name)
{
    return { .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
             .pNext = nullptr,
             .pApplicationName = name,
             .applicationVersion = VK_MAKE_VERSION(1, 33, 7),
             .pEngineName = "engine",
             .engineVersion = VK_MAKE_VERSION(1, 0, 0),
             .apiVersion = VK_API_VERSION_1_3 };
}
constexpr VkInstanceCreateInfo
instance_create_info(const VkApplicationInfo* appInfo, const std::vector<const char*>& layers, const std::vector<const char*>& extensions)
{
    return { .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
             .pNext = nullptr,
             .flags = NULL,
             .pApplicationInfo = appInfo,
             .enabledLayerCount = static_cast<uint32_t>(layers.size()),
             .ppEnabledLayerNames = layers.empty() ? nullptr : layers.data(),
             .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
             .ppEnabledExtensionNames = extensions.empty() ? nullptr : extensions.data() };
}
constexpr VkPhysicalDeviceFeatures2 physical_device_features_2(VkPhysicalDeviceVulkan11Features* pFeatures11 = nullptr)
{
    return VkPhysicalDeviceFeatures2{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = pFeatures11 };
}
constexpr VkPhysicalDeviceVulkan11Features physical_device_features_vulkan_11(VkPhysicalDeviceVulkan12Features* pFeatures12 = nullptr)
{
    return VkPhysicalDeviceVulkan11Features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, .pNext = pFeatures12 };
}
constexpr VkPhysicalDeviceVulkan12Features physical_device_features_vulkan_12()
{
    return VkPhysicalDeviceVulkan12Features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, .pNext = nullptr };
}
constexpr VkPhysicalDeviceProperties2 physical_device_properies_2(VkPhysicalDeviceVulkan11Properties* pProperties11 = nullptr)
{
    return VkPhysicalDeviceProperties2{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, .pNext = pProperties11 };
}
constexpr VkPhysicalDeviceVulkan11Properties
physical_device_properies_vulkan_11(VkPhysicalDeviceVulkan12Properties* pProperties12 = nullptr)
{
    return VkPhysicalDeviceVulkan11Properties{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES, .pNext = pProperties12 };
}
constexpr VkPhysicalDeviceVulkan12Properties physical_device_properies_vulkan_12()
{
    return VkPhysicalDeviceVulkan12Properties{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES, .pNext = nullptr };
}
template<uint32_t SIZE>
constexpr VkDeviceCreateInfo device_create_info(const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfo,
                                                const std::array<const char*, SIZE>& extensions,
                                                const VkPhysicalDeviceFeatures2& deviceFeatures2)
{
    return { .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
             .pNext = &deviceFeatures2,
             .flags = NULL,
             .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfo.size()),
             .pQueueCreateInfos = queueCreateInfo.empty() ? nullptr : queueCreateInfo.data(),
             .enabledLayerCount = NULL,    // This is deprecated and ignored
             .ppEnabledLayerNames = nullptr,
             .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
             .ppEnabledExtensionNames = extensions.empty() ? nullptr : extensions.data(),
             .pEnabledFeatures = nullptr };
}
constexpr VkDeviceQueueCreateInfo device_queue_create_info(uint32_t queueIndex, uint32_t queueCount, const float* pQueuePrio)
{
    return { .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
             .pNext = nullptr,
             .flags = NULL,
             .queueFamilyIndex = queueIndex,
             .queueCount = queueCount,
             .pQueuePriorities = pQueuePrio };
}
constexpr VkSwapchainCreateInfoKHR swapchain_create_info(VkSurfaceKHR surface,
                                                         uint32_t imageCount,
                                                         VkPresentModeKHR presentMode,
                                                         VkFormat format,
                                                         VkColorSpaceKHR colorspace,
                                                         const VkExtent2D& extent,
                                                         VkSurfaceTransformFlagBitsKHR transform,
                                                         VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE)
{
    return { .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
             .pNext = nullptr,
             .flags = NULL,
             .surface = surface,
             .minImageCount = imageCount,
             .imageFormat = format,
             .imageColorSpace = colorspace,
             .imageExtent = extent,
             .imageArrayLayers = 1u,
             .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
             .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
             .queueFamilyIndexCount = 0u,
             .pQueueFamilyIndices = nullptr,
             .preTransform = transform,
             .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
             .presentMode = presentMode,
             .clipped = VK_TRUE,
             .oldSwapchain = oldSwapchain };
}
constexpr VkImageViewCreateInfo image_view_create_info(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
    return {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .components = { .r = VK_COMPONENT_SWIZZLE_R,
                       .g = VK_COMPONENT_SWIZZLE_G,
                       .b = VK_COMPONENT_SWIZZLE_B,
                       .a = VK_COMPONENT_SWIZZLE_A },
        .subresourceRange = { .aspectMask = aspectFlags, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1 }
    };
}
constexpr VkShaderModuleCreateInfo shader_module_create_info(const std::vector<uint32_t>& codeBuffer)
{
    return { .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
             .pNext = nullptr,
             .flags = NULL,    // Unused as of vulkan 1.3
             .codeSize = codeBuffer.size() * sizeof(uint32_t),
             .pCode = codeBuffer.data() };
}
constexpr VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info(VkShaderStageFlagBits stageFlags, VkShaderModule shaderModule)
{
    return { .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
             .pNext = nullptr,
             .flags = NULL,
             .stage = stageFlags,
             .module = shaderModule,
             .pName = "main",
             .pSpecializationInfo = nullptr };
}
constexpr VkPipelineVertexInputStateCreateInfo
pipeline_vertex_input_state_create_info(const std::vector<VkVertexInputBindingDescription>& bindingDescription,
                                        const std::vector<VkVertexInputAttributeDescription>& attributeDescription)
{
    return { .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
             .pNext = nullptr,
             .flags = NULL,
             .vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescription.size()),
             .pVertexBindingDescriptions = bindingDescription.empty() ? nullptr : bindingDescription.data(),
             .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size()),
             .pVertexAttributeDescriptions = attributeDescription.empty() ? nullptr : attributeDescription.data() };
}
constexpr VkPipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_create_info(VkPrimitiveTopology topology)
{
    return {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = NULL,
        .topology = topology,
        .primitiveRestartEnable = VK_FALSE    // This is tied to drawing vertices from indices.. TODO: look into it
    };
}
constexpr VkPipelineDepthStencilStateCreateInfo pipeline_depth_stencil_create_info(bool pDepthTest, bool pDepthWrite, VkCompareOp compareOp)
{
    return {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.pNext = nullptr,
		.depthTestEnable = pDepthTest ? VK_TRUE : VK_FALSE,
		.depthWriteEnable = pDepthWrite ? VK_TRUE : VK_FALSE,
		.depthCompareOp = pDepthTest ? compareOp : VK_COMPARE_OP_ALWAYS,
		.depthBoundsTestEnable = VK_FALSE,
		.stencilTestEnable = VK_FALSE,
		.front = {
			.failOp = VK_STENCIL_OP_KEEP,
			.passOp = VK_STENCIL_OP_KEEP,
			.depthFailOp = VK_STENCIL_OP_KEEP,
			.compareOp = VK_COMPARE_OP_ALWAYS,
		},
		.back = {
			.failOp = VK_STENCIL_OP_KEEP,
			.passOp = VK_STENCIL_OP_KEEP,
			.depthFailOp = VK_STENCIL_OP_KEEP,
			.compareOp = VK_COMPARE_OP_ALWAYS,
		},
		.minDepthBounds = 0.0f, // Optional
		.maxDepthBounds = 1.0f // Optional
	};
}
constexpr VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_create_info(VkPolygonMode polygonMode)
{
    return { .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
             .pNext = nullptr,
             .flags = NULL,
             .depthClampEnable = VK_FALSE,
             .rasterizerDiscardEnable = VK_FALSE,
             .polygonMode = polygonMode,
             .cullMode = VK_CULL_MODE_NONE,
             .frontFace = VK_FRONT_FACE_CLOCKWISE,
             .depthBiasEnable = VK_FALSE,
             .depthBiasConstantFactor = 0.0f,
             .depthBiasClamp = 0.0f,
             .depthBiasSlopeFactor = 0.0f,
             .lineWidth = 1.0f };
}
constexpr VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info()
{
    return { .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
             .pNext = nullptr,
             .flags = NULL,    // unused as of vulkan 1.3
             .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
             .sampleShadingEnable = VK_FALSE,
             .minSampleShading = 1.0f,
             .pSampleMask = nullptr,
             .alphaToCoverageEnable = VK_FALSE,
             .alphaToOneEnable = VK_FALSE };
}
constexpr VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_state()
{
    return { .blendEnable = VK_FALSE,
             .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
             .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
             .colorBlendOp = VK_BLEND_OP_ADD,
             .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
             .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
             .alphaBlendOp = VK_BLEND_OP_ADD,
             .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT };
}
constexpr VkPipelineColorBlendStateCreateInfo
pipeline_color_blend_state_create_info(const std::vector<VkPipelineColorBlendAttachmentState>& colorblend)
{
    return { .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
             .pNext = nullptr,

             .flags = NULL,    // Additional color blending information -> VkPipelineColorBlendStateCreateFlagBits. Seems to be for arm?
             .logicOpEnable = VK_FALSE,
             .logicOp = VK_LOGIC_OP_COPY,
             .attachmentCount = static_cast<uint32_t>(colorblend.size()),
             .pAttachments = colorblend.empty() ? nullptr : colorblend.data() };
}
constexpr VkPipelineLayoutCreateInfo pipeline_layout_create_info(const std::vector<VkPushConstantRange>& pushConstantRanges,
                                                                 const std::vector<VkDescriptorSetLayout>& descriptorLayouts)
{
    return { .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
             .pNext = nullptr,
             .flags = NULL,
             .setLayoutCount = static_cast<uint32_t>(descriptorLayouts.size()),
             .pSetLayouts = descriptorLayouts.empty() ? nullptr : descriptorLayouts.data(),
             .pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size()),
             .pPushConstantRanges = pushConstantRanges.empty() ? nullptr : pushConstantRanges.data() };
}
constexpr VkRenderPassCreateInfo render_pass_create_info(const std::vector<VkAttachmentDescription>& attachmentDescriptions,
                                                         const std::vector<VkSubpassDescription>& subpassDescriptions,
                                                         const std::vector<VkSubpassDependency>& subpassDependencies)
{
    return { .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
             .pNext = nullptr,
             .attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size()),
             .pAttachments = attachmentDescriptions.data(),    // Pointer to the first element in our array of attachments
             .subpassCount = static_cast<uint32_t>(subpassDescriptions.size()),
             .pSubpasses = subpassDescriptions.data(),
             .dependencyCount = static_cast<uint32_t>(subpassDependencies.size()),
             .pDependencies = subpassDependencies.empty() ? nullptr : subpassDependencies.data() };
}
constexpr VkAttachmentDescription
render_pass_attachment_description(VkFormat swapchainImageFormat, VkImageLayout initialLayout, VkImageLayout finalLayout)
{
    return { .flags = NULL,
             .format = swapchainImageFormat,    // We use the format the Swapchain uses so they are compatible
             .samples = VK_SAMPLE_COUNT_1_BIT,
             .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,      // When the renderpass begins this attachment gets reset
             .storeOp = VK_ATTACHMENT_STORE_OP_STORE,    // When the renderpass ends the attachment will be stored to be presented later
             .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
             .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
             .initialLayout = initialLayout,
             .finalLayout = finalLayout };
}
constexpr VkAttachmentReference render_pass_attachment_reference(uint32_t descriptionIndex, VkImageLayout layout)
{
    return { .attachment = descriptionIndex, .layout = layout };
}
constexpr VkSubpassDescription render_pass_subpass_description(const std::vector<VkAttachmentReference>& colorAttachments,
                                                               const VkAttachmentReference& depthAttachmentReference)
{
    return { .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
             .colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size()),
             .pColorAttachments = colorAttachments.data(),
             .pDepthStencilAttachment = &depthAttachmentReference };
}
constexpr VkImageCreateInfo image_create_info(VkFormat format, VkImageUsageFlags usageFlags, const VkExtent3D& extent)
{
    return { .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
             .pNext = nullptr,
             .flags = NULL,
             .imageType = VK_IMAGE_TYPE_2D,
             .format = format,
             .extent = extent,
             .mipLevels = 1,
             .arrayLayers = 1,
             .samples = VK_SAMPLE_COUNT_1_BIT,
             .tiling = VK_IMAGE_TILING_OPTIMAL,
             .usage = usageFlags,
             .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
             .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED };
}
constexpr VkCommandBufferAllocateInfo command_buffer_allocate_info(VkCommandPool pool, uint32_t bufferCount = 1u)
{
    return { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
             .pNext = nullptr,

             .commandPool = pool,
             .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
             .commandBufferCount = bufferCount };
}
constexpr VkFenceCreateInfo fence_create_info(bool startSignaled)
{
    return { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .pNext = nullptr, .flags = startSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0u };
}
constexpr VkDescriptorSetLayoutBinding
descriptor_set_layout_binding(uint32_t binding, VkShaderStageFlags stageFlags, VkDescriptorType type, uint32_t descriptorCount = 1u)
{
    return { .binding = binding,
             .descriptorType = type,
             .descriptorCount = descriptorCount,
             .stageFlags = stageFlags,
             .pImmutableSamplers = nullptr };
}
template<uint32_t SIZE>
constexpr VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info(const std::array<VkDescriptorSetLayoutBinding, SIZE>& bindings,
                                                                            VkDescriptorSetLayoutCreateFlags flags = NULL)
{
    return { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
             .pNext = nullptr,
             .flags = flags,
             .bindingCount = static_cast<uint32_t>(bindings.size()),
             .pBindings = bindings.data() };
}
constexpr VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info(const std::vector<VkDescriptorSetLayoutBinding>& bindings,
                                                                            VkDescriptorSetLayoutCreateFlags flags = NULL)
{
    return { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
             .pNext = nullptr,
             .flags = flags,
             .bindingCount = static_cast<uint32_t>(bindings.size()),
             .pBindings = bindings.data() };
}
constexpr VkDescriptorSetAllocateInfo descriptor_set_allocate_info(VkDescriptorPool descriptorPool,
                                                                   const std::vector<VkDescriptorSetLayout>& layouts)
{
    return { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
             .pNext = nullptr,
             .descriptorPool = descriptorPool,
             .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
             .pSetLayouts = layouts.data() };
}
constexpr VkWriteDescriptorSet
write_descriptor_buffer(VkDescriptorType type, VkDescriptorSet dstSet, const VkDescriptorBufferInfo& bufferInfo, uint32_t binding)
{
    return { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,
             .dstSet = dstSet,
             .dstBinding = binding,
             .descriptorCount = 1,
             .descriptorType = type,
             .pBufferInfo = &bufferInfo };
}
constexpr VkSamplerCreateInfo sampler_create_info(VkFilter filter, VkSamplerAddressMode samplerAddressMode, VkSamplerMipmapMode mipMapMode)
{
    return { .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
             .pNext = nullptr,
             .flags = NULL,
             .magFilter = filter,
             .minFilter = filter,
             .mipmapMode = mipMapMode,
             .addressModeU = samplerAddressMode,
             .addressModeV = samplerAddressMode,
             .addressModeW = samplerAddressMode,
             .mipLodBias = 0.0f,
             .anisotropyEnable = VK_FALSE,
             .maxAnisotropy = 0.0f,
             .compareEnable = VK_FALSE,
             .compareOp = VK_COMPARE_OP_NEVER,
             .minLod = 0.0f,
             .maxLod = 0.0f,
             .borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
             .unnormalizedCoordinates = VK_FALSE };
}
constexpr VkImageSubresourceRange image_subresource_range(VkImageAspectFlags aspectFlags)
{
    return VkImageSubresourceRange{ .aspectMask = aspectFlags,
                                    .baseMipLevel = 0u,
                                    .levelCount = 1u,
                                    .baseArrayLayer = 0u,
                                    .layerCount = 1u };
}
constexpr VkImageMemoryBarrier image_memory_barrier(VkImage image,
                                                    const VkImageSubresourceRange& range,
                                                    VkImageLayout oldLayout,
                                                    VkImageLayout newLayout,
                                                    VkAccessFlags srcAccessMask,
                                                    VkAccessFlags dstAccessMask)
{
    return { .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
             .pNext = nullptr,
             .srcAccessMask = srcAccessMask,
             .dstAccessMask = dstAccessMask,
             .oldLayout = oldLayout,
             .newLayout = newLayout,
             .image = image,
             .subresourceRange = range };
}
constexpr VkBufferImageCopy image_buffer_copy(VkImageAspectFlags aspectFlags, const VkExtent3D& imageExtent)
{
    return {
		.bufferOffset = 0u,
		.bufferRowLength = 0u,
		.bufferImageHeight = 0u,
		.imageSubresource = {
			.aspectMask = aspectFlags,
			.mipLevel = 0u,
			.baseArrayLayer = 0u,
			.layerCount = 1u,
		},
		.imageExtent = imageExtent
	};
}
constexpr VkWriteDescriptorSet
write_descriptor_image(VkDescriptorType type, VkDescriptorSet dstSet, const std::vector<VkDescriptorImageInfo>& imageInfo, uint32_t binding)
{
    return { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             .pNext = nullptr,

             .dstSet = dstSet,
             .dstBinding = binding,
             .descriptorCount = static_cast<uint32_t>(imageInfo.size()),
             .descriptorType = type,
             .pImageInfo = imageInfo.data() };
}
constexpr VkQueryPoolCreateInfo query_pool_create_info(VkQueryType type, uint32_t count, VkQueryPipelineStatisticFlags statistics = 0u)
{
    return VkQueryPoolCreateInfo{ .sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,
                                  .pNext = nullptr,
                                  .flags = 0u,    // flags reserrved for future use as of VK 1.3
                                  .queryType = type,
                                  .queryCount = count,
                                  .pipelineStatistics = statistics };
}
constexpr VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info(PFN_vkDebugUtilsMessengerCallbackEXT callback)
{
    return { .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
             .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
             .messageType =
                 //VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
             .pfnUserCallback = callback,
             .pUserData = nullptr };
}
}    // namespace odin::graphics::vk
