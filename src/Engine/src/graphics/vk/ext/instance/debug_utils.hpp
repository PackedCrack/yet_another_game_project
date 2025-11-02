//
// Created by qwerty on 11/08/2025.
//
#pragma once

#include "../../Instance.hpp"
//
//
namespace odin::graphics::vk::ext::instance
{
// https://docs.vulkan.org/refpages/latest/refpages/source/VK_EXT_debug_utils.html
inline PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabel = nullptr;
inline PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabel = nullptr;
inline PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabel = nullptr;
inline PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger = nullptr;
inline PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger = nullptr;
inline PFN_vkQueueBeginDebugUtilsLabelEXT vkQueueBeginDebugUtilsLabel = nullptr;
inline PFN_vkQueueEndDebugUtilsLabelEXT vkQueueEndDebugUtilsLabel = nullptr;
inline PFN_vkQueueInsertDebugUtilsLabelEXT vkQueueInsertDebugUtilsLabel = nullptr;
inline PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectName = nullptr;
inline PFN_vkSetDebugUtilsObjectTagEXT vkSetDebugUtilsObjectTag = nullptr;
inline PFN_vkSubmitDebugUtilsMessageEXT vkSubmitDebugUtilsMessage = nullptr;
void load_debug_utils(InstanceRef instance);
}    // namespace odin::graphics::vk::ext::instance
