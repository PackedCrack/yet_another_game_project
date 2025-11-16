#include "extended_dynamic_state3.hpp"

#include "get_device_procedure.hpp"
//
//
namespace odin::graphics::vk::ext::device
{
void load_extended_dynamic_state3(DeviceRef device)
{
    vkCmdSetAlphaToCoverageEnable = get_procedure_address<PFN_vkCmdSetAlphaToCoverageEnableEXT>(device, "vkCmdSetAlphaToCoverageEnableEXT");
    vkCmdSetAlphaToOneEnable = get_procedure_address<PFN_vkCmdSetAlphaToOneEnableEXT>(device, "vkCmdSetAlphaToOneEnableEXT");
    vkCmdSetColorBlendEnable = get_procedure_address<PFN_vkCmdSetColorBlendEnableEXT>(device, "vkCmdSetColorBlendEnableEXT");
    vkCmdSetColorBlendEquation = get_procedure_address<PFN_vkCmdSetColorBlendEquationEXT>(device, "vkCmdSetColorBlendEquationEXT");
    vkCmdSetColorWriteMask = get_procedure_address<PFN_vkCmdSetColorWriteMaskEXT>(device, "vkCmdSetColorWriteMaskEXT");
    vkCmdSetDepthClampEnable = get_procedure_address<PFN_vkCmdSetDepthClampEnableEXT>(device, "vkCmdSetDepthClampEnableEXT");
    vkCmdSetLogicOpEnable = get_procedure_address<PFN_vkCmdSetLogicOpEnableEXT>(device, "vkCmdSetLogicOpEnableEXT");
    vkCmdSetPolygonMode = get_procedure_address<PFN_vkCmdSetPolygonModeEXT>(device, "vkCmdSetPolygonModeEXT");
    vkCmdSetRasterizationSamples = get_procedure_address<PFN_vkCmdSetRasterizationSamplesEXT>(device, "vkCmdSetRasterizationSamplesEXT");
    vkCmdSetSampleMask = get_procedure_address<PFN_vkCmdSetSampleMaskEXT>(device, "vkCmdSetSampleMaskEXT");
}
}    //namespace odin::graphics::vk::ext::device
