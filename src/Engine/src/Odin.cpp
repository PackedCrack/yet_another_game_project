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
