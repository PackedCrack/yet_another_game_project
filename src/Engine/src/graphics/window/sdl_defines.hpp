//
// Created by qwerty on 10/08/2025.
//
#pragma once

#include "debug/Logger.hpp"
// sdl
#include "SDL3/SDL_error.h"


#ifndef NDEBUG
    #define SDL_CHECK(expr, ...)                                                                                                           \
        if (expr == 0)                                                                                                                     \
        {}                                                                                                                                 \
        else                                                                                                                               \
        {                                                                                                                                  \
            LOG_ERR(__VA_ARGS__);                                                                                                          \
            LOG_ERR("SDL Check failed with {}.", SDL_GetError());                                                                          \
            ODIN_ASSERT(expr != 0);                                                                                                        \
        }
#else
    #define SDL_CHECK(expr, ...)                                                                                                           \
        if (expr == 0)                                                                                                                     \
        {}                                                                                                                                 \
        else                                                                                                                               \
        {                                                                                                                                  \
            LOG_ERR(__VA_ARGS__);                                                                                                          \
            LOG_FATAL("SDL Check failed with {}.", SDL_GetError());                                                                        \
        }
#endif
