//
// Created by qwerty on 11/08/2025.
//
#include "DebugMessenger.hpp"

#include "debug/Logger.hpp"
#include "vulkan_info.hpp"
//
//
namespace
{
[[nodiscard]] std::string format_debug_message(const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData)
{
    // Format the message to make it more readable..
    std::string msg;
    std::string tmpStr = {};
    uint32_t index = 0;
    // This loop is beyond retarded.. TODO: fix it
    while (pCallbackData->pMessage[index] != '\0')
    {
        if (pCallbackData->pMessage[index] == ' ')
        {
            if (pCallbackData->pMessage[index + 1] == '(' && pCallbackData->pMessage[index + 2] == 'h')
            {
                tmpStr += '\n';
            }
        }

        tmpStr += pCallbackData->pMessage[index];

        // should reset
        if (pCallbackData->pMessage[index] == ':')
        {
            // hack

            if (tmpStr == "Validation Error:")
            {
                while (pCallbackData->pMessage[index] != ']')
                {
                    ++index;
                    tmpStr += pCallbackData->pMessage[index];
                }
            }
            if (pCallbackData->pMessage[index + 1] != '/')
            {
                msg += tmpStr + '\n';
                tmpStr.clear();
                // Ignore space
                ++index;
            }
        }

        ++index;
    }
    msg += tmpStr;

    return msg;
}
[[nodiscard]] VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug_message_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                                           [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                                           const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                                           [[maybe_unused]] void* pUserData)
{
    std::string message = format_debug_message(pCallbackData);

    // Use regular logger for now - in the future use vulkan specific formatting perhaps..

    //debug::LoggerLevel level{};
    // Diagnostic message
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
    {
        LOG_DEBUG(message);
        //level = debug::LoggerLevel::debug;
    }
    // Informational message like the creation of a resource
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    {
        LOG_INFO(message);
        //level = debug::LoggerLevel::info;
    }
    // Message about behavior that is not necessarily an error, but very likely a bug
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        LOG_WARN(message);
        //level = debug::LoggerLevel::warn;
    }
    // Message about behavior that is invalid and may cause crashes
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        LOG_ERR(message);
        //level = debug::LoggerLevel::error;
    }
    //const std::string msg = pCallbackData->pMessage;
    //logger::vk_validation(level, msg);

    return VK_FALSE;
};
}    // namespace
namespace odin::graphics::vk
{
DebugMessenger::DebugMessenger(const Instance& instance)
    : m_Procedures{ instance }
    , m_Messenger{ create_debug_messenger() }
{}
DebugMessenger::~DebugMessenger()
{
    if (m_Messenger != VK_NULL_HANDLE)
    {
        m_Procedures.destroy_debug_utils_messenger(m_Messenger);
    }
}
DebugMessenger::DebugMessenger(DebugMessenger&& other) noexcept
    : m_Procedures{ std::move(other.m_Procedures) }
    , m_Messenger{ VK_NULL_HANDLE }
{
    std::swap(m_Messenger, other.m_Messenger);
}
DebugMessenger& DebugMessenger::operator=(DebugMessenger&& other) noexcept
{
    if (this != std::addressof(other))
    {
        m_Procedures = std::move(other.m_Procedures);
        m_Messenger = std::exchange(other.m_Messenger, m_Messenger);
    }

    return *this;
}
VkDebugUtilsMessengerEXT DebugMessenger::create_debug_messenger() const
{
    return m_Procedures.create_debug_utils_messenger(vk::debug_messenger_create_info(vulkan_debug_message_callback));
}
}    // namespace odin::graphics::vk
