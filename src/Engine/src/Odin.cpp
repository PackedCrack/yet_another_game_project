//
// Created by qwerty on 11/08/2025.
//
#include "Odin.hpp"
//
//
namespace odin
{
Odin::Odin(OdinInfo info)
    : m_Gfx{ info }
{}
const graphics::Graphics& Odin::graphics() const
{
    return m_Gfx;
}
}    // namespace odin
