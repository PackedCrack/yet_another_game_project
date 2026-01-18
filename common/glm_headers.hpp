//
// Created by qwerty on 22/07/2024.
//
#pragma once

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wsign-conversion"
    #pragma clang diagnostic ignored "-Wold-style-cast"
#elif _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 4127)     // conditional expression is constant
    #pragma warning(disable: 4201)     // nameless struct/union
    #pragma warning(disable: 26495)    // Variable is uninitialized
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#ifdef __clang__
    #pragma clang diagnostic pop
#elif _MSC_VER
    #pragma warning(pop)
#endif
