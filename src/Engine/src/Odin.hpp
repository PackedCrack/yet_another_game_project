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
    [[nodiscard]] const graphics::Graphics& graphics() const;
private:
    graphics::Graphics m_Gfx;
};
}    // namespace odin
