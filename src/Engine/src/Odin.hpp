//
// Created by qwerty on 11/08/2025.
//
#pragma once

#include "OdinInfo.hpp"
#include "graphics/Graphics.hpp"
//
//
namespace odin
{
class Odin
{
public:
    Odin(OdinInfo info);
public:
    [[nodiscard]] graphics::Graphics& graphics();
private:
    graphics::Graphics m_Gfx;
};
}    // namespace odin
