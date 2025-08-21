//
// Created by qwerty on 21/08/2025.
//
//
// This is shared between cpp and glsl
//
#ifdef __cplusplus
    #pragma once

    #include <glm/glm.hpp>
    #include <glm/gtc/quaternion.hpp>

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
