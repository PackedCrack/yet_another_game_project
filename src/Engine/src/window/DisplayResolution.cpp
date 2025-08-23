//
// Created by qwerty on 11/08/2025.
//
#include "DisplayResolution.hpp"
//
//
namespace odin::window
{
HDPlus::HDPlus()
    : details::Resolution<HDPlus>(1600, 900)
{}
FHD::FHD()
    : details::Resolution<FHD>(1920, 1080)
{}
}    // namespace odin::window
