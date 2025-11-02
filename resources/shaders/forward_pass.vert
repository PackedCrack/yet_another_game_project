#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_ARB_shader_draw_parameters : require

#include "gpu_types.hpp"


layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;

layout (location = 1) out vec3 out_Normal;

layout(std140, set = INDIRECT_SET_ID, binding = INDIRECT_SET_BIND_ID_CAMERA_DATA) uniform CameraData
{
    CameraInfo camera;
};

void main() 
{
    out_Normal = in_Normal;
    gl_Position = camera.viewproj * vec4(in_Position, 1.0);
}