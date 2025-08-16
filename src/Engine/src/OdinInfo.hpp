//
// Created by qwerty on 11/08/2025.
//
#pragma once

#include "graphics/window/DisplayResolution.hpp"
//
//
namespace odin
{
struct WindowInfo
{
    graphics::window::DisplayResolution resolution;
    bool borderless;
    bool fullscreen;
    bool mouseGrab;
};
struct OdinInfo
{
    std::string applicationName;
    WindowInfo windowInfo;
};
}    // namespace odin
