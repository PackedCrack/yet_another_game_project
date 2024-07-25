//
// Created by qwerty on 25/07/2024.
//

#pragma once

#include "tinygltf/tiny_gltf.h"
//
//
namespace asl
{
enum class AlphaMode
{
    opaque,
    mask,
    blend
};
struct Alpha
{
    double alphaCutoff;
    AlphaMode mode;
};
enum class MinFilter
{
    nearest = TINYGLTF_TEXTURE_FILTER_NEAREST,
    linear = TINYGLTF_TEXTURE_FILTER_LINEAR,
    nearestMipMapNearest = TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST,
    linearMipMapNearest = TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST,
    nearestMipMapLinear = TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR,
    linearMipMapLinear = TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR
};
enum class MagFilter
{
    nearest = TINYGLTF_TEXTURE_FILTER_NEAREST,
    linear = TINYGLTF_TEXTURE_FILTER_LINEAR
};
enum class Wrapping
{
    repeat = TINYGLTF_TEXTURE_WRAP_REPEAT,
    clampToEdge = TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE,
    mirroredRepeat = TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT
};
struct Sampler
{
    Wrapping wrapS;    // U coordinate
    Wrapping wrapT;    // V coordinate
    std::optional<MinFilter> minfilter;
    std::optional<MagFilter> magfilter;
};
struct TextureData
{
    std::string texturKey;
    std::vector<uint8_t> imageBuffer;
    int32_t imageWidth;
    int32_t imageHeight;
    Sampler sampler;
};
struct BaseTexture
{
    std::vector<double> factor;
    std::optional<TextureData> texture;
};
struct MetallicRoughnessTexture
{
    double metallic;
    double roughness;
    std::optional<TextureData> texture;
};
struct NormalTexture
{
    double scale;    // scaledNormal = normalize((<sampled normal texture value> * 2.0 - 1.0) * vec3(<normal scale>, <normal scale>, 1.0))
    std::optional<TextureData> texture;
};
struct OcclusionTexture
{
    double strength;    // occludedColor = lerp(color, color * <sampled occlusion texture value>, <occlusion strength>)
    std::optional<TextureData> texture;
};
struct EmissiveTexture
{
    std::vector<double> factor;
    std::optional<TextureData> texture;
};
class Material
{
public:
    Material(const tinygltf::Model& model, const tinygltf::Material& material);
private:
    Alpha m_AlphaSettings;
    BaseTexture m_Base;
    MetallicRoughnessTexture m_MetalRough;
    NormalTexture m_Normal;
    OcclusionTexture m_Occlusion;
    EmissiveTexture m_Emissive;
    bool m_DoubleSided;
};
}    // namespace asl
