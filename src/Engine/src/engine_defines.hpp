#pragma once

#define NO_FLAGS 0

#ifdef __clang__
inline void debugbreak()
{
    __asm __volatile__("int3");
}
    #define DEBUG_BREAK debugbreak()
#elif _MSC_VER
    #define DEBUG_BREAK __debugbreak()
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
