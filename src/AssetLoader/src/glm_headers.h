//
// Created by qwerty on 22/07/2024.
//

#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_CTOR_INIT
#define GLM_FORCE_SSE2
#ifndef NDEBUG
    #define GLM_FORCE_XYZW_ONLY
#endif
//#define GLM_FORCE_ALIGNED


#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wsign-conversion"
    #pragma clang diagnostic ignored "-Wold-style-cast"
#elif _MSC_VER
    #pragma warning(disable: 4'201)     // nameless struct/union
    #pragma warning(disable: 26'495)    // Variable is uninitialized
#endif

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/quaternion.hpp"

#ifdef __clang__
    #pragma clang diagnostic pop
#elif _MSC_VER
    #pragma warning(enable: 4'201)     // nameless struct/union
    #pragma warning(enable: 26'495)    // Variable is uninitialized
#endif
