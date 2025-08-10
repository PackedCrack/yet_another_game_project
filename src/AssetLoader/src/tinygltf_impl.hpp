#pragma once
// wrapper header in order to turn of warnings for third party library

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wsign-conversion"
    #pragma clang diagnostic ignored "-Wold-style-cast"
#endif

#include "tinygltf/tiny_gltf.h"

#ifdef __clang__
    #pragma clang diagnostic pop
#endif
