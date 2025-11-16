#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require


layout (location = 0) in vec3 in_Normal;

layout(location = 0) out vec4 finalColor;


void main() 
{
    vec3 n = normalize(in_Normal) * 0.5 + 0.5;
    finalColor = vec4(n, 1.0);
}