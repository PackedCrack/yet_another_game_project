//
// Created by qwerty on 21/08/2025.
//
//
// This is shared between cpp and glsl
//
#ifdef __cplusplus
    #pragma once

    #include <glm_headers.hpp>

    #define int std::int32_t
    #define uint std::uint32_t
    #define vec3 glm::vec3
    #define vec4 glm::vec4
    #define mat4 glm::mat4
    #define quat glm::quat

namespace odin::graphics
{
#endif
//
//
#define DRAW_FLAG_HAS_NORMAL 1 << 0
#define DRAW_FLAG_HAS_TANGENT 1 << 1
#define DRAW_FLAG_HAS_TEXCOORD_0 1 << 2
#define DRAW_FLAG_HAS_TEXCOORD_1 1 << 3

#define GLOBAL_SET_ID 0
#define GLOBAL_SET_BIND_ID_MATERIAL_TABLE 0
#define GLOBAL_SET_BIND_ID_MESH_INFO 1


#define INDIRECT_SET_LOCAL_SIZE_X 64
#define INDIRECT_SET_ID 1
#define INDIRECT_SET_BIND_ID_DRAW_COUNT 0
#define INDIRECT_SET_BIND_ID_DRAW_ARGS 1
#define INDIRECT_SET_BIND_ID_DRAW_INSTANCE_BASE 2
#define INDIRECT_SET_BIND_ID_DRAW_INSTANCE_COUNTER 3
#define INDIRECT_SET_BIND_ID_DRAW_INSTANCE_INDEX 4
#define INDIRECT_SET_BIND_ID_INSTANCE_INFO 5
#define INDIRECT_SET_BIND_ID_CAMERA_DATA 6
//
//
struct InstanceInfo
{
    vec3 translation;
    float scale;
    quat orientation;
    int meshID;
};
struct MeshInfo
{
    uint indexCount;
    uint firstIndex;
    int vertexOffset;
    uint materialID;
    uint drawFlags;
};
struct Material
{
    int albedoID;
    int normalID;
    int metalRoughID;
    int occlusionID;
    int emissiveID;
    float rougness;
    float metallic;
};
struct CameraInfo
{
    mat4 proj;
    mat4 view;
    mat4 viewproj;
};
struct SceneInfo
{
    vec4 ambientLight;
    vec4 sunlight;
};
#ifdef __cplusplus
}    // namespace odin::graphics
#endif
#undef int
#undef uint
#undef vec3
#undef vec4
#undef mat4
#undef quat
