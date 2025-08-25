//
// Created by qwerty on 25/08/2025.
//
#pragma once

#include "../InputEvent.hpp"
//
//
namespace odin::state
{
struct Input
{
    KeyboardEvent keyboard;
    MouseClickEvent mouseClick;
    MouseMotionEvent mouseMotion;
};
}    // namespace odin::state
