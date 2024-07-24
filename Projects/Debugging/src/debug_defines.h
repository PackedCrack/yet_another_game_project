#pragma once

#define NO_FLAGS 0

#ifdef __clang__
inline void debugbreak()
{
    __asm __volatile__("int3");
}
    #define DEBUG_BREAK debugbreak()
    #define UNHANDLED_CASE_PROTECTION_ON _Pragma("clang diagnostic error \"-Wswitch-enum\"")
    #define UNHANDLED_CASE_PROTECTION_OFF _Pragma("clang diagnostic warning \"-Wswitch-enum\"")
#elif _MSC_VER
    #define DEBUG_BREAK __debugbreak()
    #define UNHANDLED_CASE_PROTECTION_ON __pragma(warning(error : Wswitch))
    #define UNHANDLED_CASE_PROTECTION_OFF __pragma(warning(default : Wswitch))
#else
    #error Unsupported compiler
#endif

#ifndef NDEBUG
    #define ODIN_ASSERT(expr)                                                                                                              \
        if (expr)                                                                                                                          \
        {}                                                                                                                                 \
        else                                                                                                                               \
        {                                                                                                                                  \
            DEBUG_BREAK;                                                                                                                   \
        }
    #define ODIN_CHECK(expr)                                                                                                               \
        if (expr)                                                                                                                          \
        {}                                                                                                                                 \
        else                                                                                                                               \
        {                                                                                                                                  \
            LOG_FATAL(std::to_string(expr));                                                                                               \
            DEBUG_BREAK;                                                                                                                   \
        }
#else
    #define ODIN_ASSERT(expr)
    #define ODIN_CHECK(expr) expr
#endif
