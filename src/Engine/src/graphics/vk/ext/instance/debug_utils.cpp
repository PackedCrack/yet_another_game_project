//
// Created by qwerty on 11/08/2025.
//
#include "debug_utils.hpp"

#include "get_instance_procedure.hpp"
//
//
namespace odin::graphics::vk::ext::instance
{
void load_debug_utils(InstanceRef instance)
{
    vkCmdBeginDebugUtilsLabel = get_procedure_address<PFN_vkCmdBeginDebugUtilsLabelEXT>(instance, "vkCmdBeginDebugUtilsLabelEXT");
    vkCmdEndDebugUtilsLabel = get_procedure_address<PFN_vkCmdEndDebugUtilsLabelEXT>(instance, "vkCmdEndDebugUtilsLabelEXT");
    vkCmdInsertDebugUtilsLabel = get_procedure_address<PFN_vkCmdInsertDebugUtilsLabelEXT>(instance, "vkCmdInsertDebugUtilsLabelEXT");
    vkCreateDebugUtilsMessenger = get_procedure_address<PFN_vkCreateDebugUtilsMessengerEXT>(instance, "vkCreateDebugUtilsMessengerEXT");
    vkDestroyDebugUtilsMessenger = get_procedure_address<PFN_vkDestroyDebugUtilsMessengerEXT>(instance, "vkDestroyDebugUtilsMessengerEXT");
    vkQueueBeginDebugUtilsLabel = get_procedure_address<PFN_vkQueueBeginDebugUtilsLabelEXT>(instance, "vkQueueBeginDebugUtilsLabelEXT");
    vkQueueEndDebugUtilsLabel = get_procedure_address<PFN_vkQueueEndDebugUtilsLabelEXT>(instance, "vkQueueEndDebugUtilsLabelEXT");
    vkQueueInsertDebugUtilsLabel = get_procedure_address<PFN_vkQueueInsertDebugUtilsLabelEXT>(instance, "vkQueueInsertDebugUtilsLabelEXT");
    vkSetDebugUtilsObjectName = get_procedure_address<PFN_vkSetDebugUtilsObjectNameEXT>(instance, "vkSetDebugUtilsObjectNameEXT");
    vkSetDebugUtilsObjectTag = get_procedure_address<PFN_vkSetDebugUtilsObjectTagEXT>(instance, "vkSetDebugUtilsObjectTagEXT");
    vkSubmitDebugUtilsMessage = get_procedure_address<PFN_vkSubmitDebugUtilsMessageEXT>(instance, "vkSubmitDebugUtilsMessageEXT");
}
}    // namespace odin::graphics::vk::ext::instance
