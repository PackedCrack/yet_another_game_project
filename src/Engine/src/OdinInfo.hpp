//
// Created by qwerty on 11/08/2025.
//
#pragma once

#include "window/DisplayResolution.hpp"
//
//
namespace odin
{
struct WindowInfo
{
    window::DisplayResolution resolution;
    bool borderless;
    bool fullscreen;
    bool mouseGrab;
};
struct OdinInfo
{
    int argc;
    char** argv;
    std::string applicationName;
    WindowInfo windowInfo;
};
}    // namespace odin
