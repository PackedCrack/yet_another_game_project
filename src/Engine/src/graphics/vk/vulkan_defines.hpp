#pragma once
//
//
#include "debug/Logger.hpp"
// vulkan
#include "vulkan/vulkan.h"
// std
#include <string>
constexpr const char* err_to_str(VkResult error)
{
    switch (error)
    {
    case VK_ERROR_OUT_OF_HOST_MEMORY:
        return "VK_ERROR_OUT_OF_HOST_MEMORY";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
    case VK_ERROR_INITIALIZATION_FAILED:
        return "VK_ERROR_INITIALIZATION_FAILED";
    case VK_ERROR_DEVICE_LOST:
        return "VK_ERROR_DEVICE_LOST";
    case VK_ERROR_MEMORY_MAP_FAILED:
        return "VK_ERROR_MEMORY_MAP_FAILED";
    case VK_ERROR_LAYER_NOT_PRESENT:
        return "VK_ERROR_LAYER_NOT_PRESENT";
    case VK_ERROR_EXTENSION_NOT_PRESENT:
        return "VK_ERROR_EXTENSION_NOT_PRESENT";
    case VK_ERROR_FEATURE_NOT_PRESENT:
        return "VK_ERROR_FEATURE_NOT_PRESENT";
    case VK_ERROR_INCOMPATIBLE_DRIVER:
        return "VK_ERROR_INCOMPATIBLE_DRIVER";
    case VK_ERROR_TOO_MANY_OBJECTS:
        return "VK_ERROR_TOO_MANY_OBJECTS";
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
        return "VK_ERROR_FORMAT_NOT_SUPPORTED";
    case VK_ERROR_FRAGMENTED_POOL:
        return "VK_ERROR_FRAGMENTED_POOL";
    case VK_ERROR_UNKNOWN:
        return "VK_ERROR_UNKNOWN";
    case VK_ERROR_OUT_OF_POOL_MEMORY:
        return "VK_ERROR_OUT_OF_POOL_MEMORY";
    case VK_ERROR_INVALID_EXTERNAL_HANDLE:
        return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
    case VK_ERROR_FRAGMENTATION:
        return "VK_ERROR_FRAGMENTATION";
    case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
        return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
    case VK_PIPELINE_COMPILE_REQUIRED:
        return "VK_PIPELINE_COMPILE_REQUIRED";
    case VK_ERROR_SURFACE_LOST_KHR:
        return "VK_ERROR_SURFACE_LOST_KHR";
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
    case VK_SUBOPTIMAL_KHR:
        return "VK_SUBOPTIMAL_KHR";
    case VK_ERROR_OUT_OF_DATE_KHR:
        return "VK_ERROR_OUT_OF_DATE_KHR";
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
        return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
    case VK_ERROR_VALIDATION_FAILED_EXT:
        return "VK_ERROR_VALIDATION_FAILED_EXT";
    case VK_ERROR_INVALID_SHADER_NV:
        return "VK_ERROR_INVALID_SHADER_NV";
    case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
        return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
    case VK_ERROR_NOT_PERMITTED_KHR:
        return "VK_ERROR_NOT_PERMITTED_KHR";
    case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
        return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
    case VK_THREAD_IDLE_KHR:
        return "VK_THREAD_IDLE_KHR";
    case VK_THREAD_DONE_KHR:
        return "VK_THREAD_DONE_KHR";
    case VK_OPERATION_DEFERRED_KHR:
        return "VK_OPERATION_DEFERRED_KHR";
    case VK_OPERATION_NOT_DEFERRED_KHR:
        return "VK_OPERATION_NOT_DEFERRED_KHR";
    case VK_SUCCESS:
        return "VK_SUCCESS";
    case VK_NOT_READY:
        return "VK_NOT_READY";
    case VK_TIMEOUT:
        return "VK_TIMEOUT";
    case VK_EVENT_SET:
        return "VK_EVENT_SET";
    case VK_EVENT_RESET:
        return "VK_EVENT_RESET";
    case VK_INCOMPLETE:
        return "VK_INCOMPLETE";
    case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:
        return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
    case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:
        return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
    case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:
        return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
    case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:
        return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
    case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:
        return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
    case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:
        return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
    case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR:
        return "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR";
    case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
        return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
    case VK_INCOMPATIBLE_SHADER_BINARY_EXT:
        return "VK_INCOMPATIBLE_SHADER_BINARY_EXT";
    case VK_PIPELINE_BINARY_MISSING_KHR:
        return "VK_PIPELINE_BINARY_MISSING_KHR";
    case VK_ERROR_NOT_ENOUGH_SPACE_KHR:
        return "VK_ERROR_NOT_ENOUGH_SPACE_KHR";
    case VK_RESULT_MAX_ENUM:
        return "VK_RESULT_MAX_ENUM";
    default:
        auto code = static_cast<std::uint32_t>(error);
        return std::format("Unknown error: {}", code).c_str();
    }
}
#ifndef NDEBUG
    #define VK_CHECK(expr, ...)                                                                                                            \
        if (VkResult result = expr; result == VK_SUCCESS)                                                                                  \
        {}                                                                                                                                 \
        else                                                                                                                               \
        {                                                                                                                                  \
            LOG_ERR(__VA_ARGS__);                                                                                                          \
            ODIN_ASSERT(result != VK_SUCCESS);                                                                                             \
        }
#else
    #define VK_CHECK(expr, ...)                                                                                                            \
        if (VkResult result = expr; result == VK_SUCCESS)                                                                                  \
        {}                                                                                                                                 \
        else                                                                                                                               \
        {                                                                                                                                  \
            LOG_ERR("Vulkan check failure, error code: {}", err_to_str(result));                                                           \
            LOG_FATAL(__VA_ARGS__);                                                                                                        \
        }
#endif

#ifndef NDEBUG
    #define VK_RESULT(expr, ...)                                                                                                           \
        if (VkResult result = expr; result >= 0)                                                                                           \
        {}                                                                                                                                 \
        else                                                                                                                               \
        {                                                                                                                                  \
            LOG_ERR(__VA_ARGS__);                                                                                                          \
            ODIN_ASSERT(result < 0);                                                                                                       \
        }
#else
    #define VK_RESULT(expr, ...)                                                                                                           \
        if (VkResult result = expr; result >= 0)                                                                                           \
        {}                                                                                                                                 \
        else                                                                                                                               \
        {                                                                                                                                  \
            LOG_ERR("Vulkan result check failure, error code: {}", err_to_str(result));                                                    \
            LOG_FATAL(__VA_ARGS__);                                                                                                        \
        }
#endif
//    #ifdef _MSC_VER
//        #define ODIN_ASSERT_VK_RESULT(expr)                                                                                                \
//            if (expr >= 0)                                                                                                                 \
//            {}                                                                                                                             \
//            else                                                                                                                           \
//            {                                                                                                                              \
//                logger::fatal("Vulkan assert failure, error code: " + err_to_str(expr), __FILE__, __func__, __LINE__);                     \
//                __debugbreak();                                                                                                            \
//            }
//        #define ODIN_ASSERT_VK_SUCCESS(expr)                                                                                               \
//            if (expr == VK_SUCCESS)                                                                                                        \
//            {}                                                                                                                             \
//            else                                                                                                                           \
//            {                                                                                                                              \
//                logger::fatal("Vulkan assert failure, error code: " + err_to_str(expr), __FILE__, __func__, __LINE__);                     \
//                __debugbreak();                                                                                                            \
//            }
//    #else
//        #define ODIN_ASSERT_VK_RESULT(expr)                                                                                                \
//            if (expr >= 0)                                                                                                                 \
//            {}                                                                                                                             \
//            else                                                                                                                           \
//            {                                                                                                                              \
//                logger::fatal("Vulkan assert failure, error code: " + err_to_str(expr), __FILE__, __func__, __LINE__);                     \
//            }
//        #define ODIN_ASSERT_VK_SUCCESS(expr)                                                                                               \
//            if (expr == VK_SUCCESS)                                                                                                        \
//            {}                                                                                                                             \
//            else                                                                                                                           \
//            {                                                                                                                              \
//                logger::fatal("Vulkan assert failure, error code: " + err_to_str(expr), __FILE__, __func__, __LINE__);                     \
//            }
//
//    #endif
//#else
//    #define ODIN_ASSERT_VK_RESULT(expr) expr;
//    #define ODIN_ASSERT_VK_SUCCESS(expr) expr;
//#endif
