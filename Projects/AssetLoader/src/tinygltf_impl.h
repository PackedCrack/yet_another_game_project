#pragma once
// wrapper header in order to turn of warnings for third party library

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wsign-conversion"
    #pragma clang diagnostic ignored "-Wold-style-cast"
#elif _MSC_VER
    #error check the warnings and add them to ignore
#endif

#include "tinygltf/tiny_gltf.h"

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wsign-conversion"
    #pragma clang diagnostic ignored "-Wold-style-cast"
#elif _MSC_VER
    #error enable the warnings again
#endif
