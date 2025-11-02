//
// Created by qwerty on 02/10/2025.
//
#pragma once


#include "../../Device.hpp"
//
//
namespace odin::graphics::vk::ext::device
{
// https://docs.vulkan.org/refpages/latest/refpages/source/VK_EXT_extended_dynamic_state3.html
inline PFN_vkCmdSetAlphaToCoverageEnableEXT vkCmdSetAlphaToCoverageEnable = nullptr;
inline PFN_vkCmdSetAlphaToOneEnableEXT vkCmdSetAlphaToOneEnable = nullptr;
inline PFN_vkCmdSetColorBlendEnableEXT vkCmdSetColorBlendEnable = nullptr;
inline PFN_vkCmdSetColorBlendEquationEXT vkCmdSetColorBlendEquation = nullptr;
inline PFN_vkCmdSetColorWriteMaskEXT vkCmdSetColorWriteMask = nullptr;
inline PFN_vkCmdSetDepthClampEnableEXT vkCmdSetDepthClampEnable = nullptr;
inline PFN_vkCmdSetLogicOpEnableEXT vkCmdSetLogicOpEnable = nullptr;
inline PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonMode = nullptr;
inline PFN_vkCmdSetRasterizationSamplesEXT vkCmdSetRasterizationSamples = nullptr;
inline PFN_vkCmdSetSampleMaskEXT vkCmdSetSampleMask = nullptr;
void load_extended_dynamic_state3(DeviceRef device);
}    //namespace odin::graphics::vk::ext::device
