#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require


layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec4 vTangent;
layout (location = 3) in vec2 vTexcoord_0;
layout (location = 4) in vec2 vTexcoord_1;

layout(location = 0) out vec3 finalColor;


void main() 
{
    finalColor = vNormal;
}