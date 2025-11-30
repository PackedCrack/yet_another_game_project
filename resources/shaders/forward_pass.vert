#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_ARB_shader_draw_parameters : require

#include "gpu_types.hpp"


layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;

layout (location = 0) out vec3 out_Normal;

layout(std140, set = INDIRECT_SET_ID, binding = INDIRECT_SET_BIND_ID_CAMERA_DATA) uniform CameraData
{
    CameraInfo camera;
};

layout(std430, set = INDIRECT_SET_ID, binding = INDIRECT_SET_BIND_ID_INSTANCE_INDEX) readonly buffer InstanceIndices
{
    uint instanceIndex[];
};

layout(std430, set = INDIRECT_SET_ID, binding = INDIRECT_SET_BIND_ID_INSTANCE_INFO) readonly buffer InstanceInfos
{
    // Declared in gpu_types.hpp
    InstanceInfo instances[];
};

vec3 rotate(vec4 quat, vec3 pos)
{
    vec3 t = 2.0 * cross(quat.xyz, pos);
    return pos + quat.w * t + cross(quat.xyz, t);
}

void main() 
{
    //uint instanceID = instanceIndex[gl_BaseInstanceARB + gl_InstanceIndex];   // gl_BaseInstanceARB + gl_InstanceIndex - Gives the wrong index
    //uint instanceID = instanceIndex[gl_DrawIDARB];  // Works - but probably not for instancing
    uint instanceID = instanceIndex[gl_InstanceIndex];  // Works
    InstanceInfo instance = instances[instanceID];

    vec4 orientation = normalize(instance.orientation);
    vec3 pos = rotate(orientation, in_Position * instance.scale) + instance.translation;
    vec3 normals = normalize(rotate(orientation, in_Normal));
    
    //if (instance.meshID == 1)
    //{
    //    pos.x = pos.x + 100.0;
    //}

    //out_Normal = normals;
    out_Normal = vec3(float(instanceID) / 2.0);
    gl_Position = camera.viewproj * vec4(pos.x, pos.y, pos.z - 50.0, 1.0);
}

