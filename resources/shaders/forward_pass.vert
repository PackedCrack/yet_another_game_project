#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_ARB_shader_draw_parameters : require

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec4 vTangent;
layout (location = 3) in vec2 vTexcoord_0;
layout (location = 4) in vec2 vTexcoord_1;

layout (location = 0) out vec3 out_Position;
layout (location = 1) out vec3 out_Normal;
layout (location = 2) out vec4 out_Tangent;
layout (location = 3) out vec2 out_Texcoord_0;
layout (location = 4) out vec2 out_Texcoord_1;

//layout(std140, set = 0, binding = 0) uniform ubo_CameraBuffer
//{
//    mat4 proj;
//	mat4 view;
//	mat4 viewProj;
//} cameraBuffer;

void main() 
{
    //gl_Position = cameraBuffer.viewProj * vec4(vPosition, 1.0);

    out_Position = vPosition;
    out_Normal = vNormal;
    out_Tangent = vTangent;
    out_Texcoord_0 = vTexcoord_0;
    out_Texcoord_1 = vTexcoord_1;
}