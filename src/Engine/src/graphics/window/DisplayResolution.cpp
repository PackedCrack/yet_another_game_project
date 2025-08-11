#include "DisplayResolution.hpp"
//
//
namespace odin::graphics::window
{
HDPlus::HDPlus()
    : details::Resolution<HDPlus>(1600, 900)
{}
FHD::FHD()
    : details::Resolution<FHD>(1920, 1080)
{}
}    // namespace odin::graphics::window
