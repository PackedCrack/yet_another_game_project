//
// Created by qwerty on 25/07/2024.
//

#pragma once

#include "tinygltf/tiny_gltf.h"
#include "common_types.hpp"
//
//
namespace asl
{
//enum class AlphaMode
//{
//    opaque,
//    mask,
//    blend
//};
//struct Alpha
//{
//    double alphaCutoff;
//    AlphaMode mode;
//};
//enum class MinFilter
//{
//    nearest = TINYGLTF_TEXTURE_FILTER_NEAREST,
//    linear = TINYGLTF_TEXTURE_FILTER_LINEAR,
//    nearestMipMapNearest = TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST,
//    linearMipMapNearest = TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST,
//    nearestMipMapLinear = TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR,
//    linearMipMapLinear = TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR
//};
//enum class MagFilter
//{
//    nearest = TINYGLTF_TEXTURE_FILTER_NEAREST,
//    linear = TINYGLTF_TEXTURE_FILTER_LINEAR
//};
//enum class Wrapping
//{
//    repeat = TINYGLTF_TEXTURE_WRAP_REPEAT,
//    clampToEdge = TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE,
//    mirroredRepeat = TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT
//};
//struct Sampler
//{
//    Wrapping wrapS;    // U coordinate
//    Wrapping wrapT;    // V coordinate
//    std::optional<MinFilter> minfilter;
//    std::optional<MagFilter> magfilter;
//};
template<typename derived_t>
struct Texture
{
    std::string textureKey;
    std::vector<uint8_t> imageBuffer;
    int32_t imageWidth;
    int32_t imageHeight;
    Sampler sampler;
};
struct Base : public Texture<Base>
{
    std::vector<double> factor;
};
struct MetallicRoughness : public Texture<MetallicRoughness>
{
    double metallic;
    double roughness;
};
struct Normal : public Texture<Normal>
{
    double scale;    // scaledNormal = normalize((<sampled normal texture value> * 2.0 - 1.0) * vec3(<normal scale>, <normal scale>, 1.0))
};
struct Occlusion : public Texture<Occlusion>
{
    double strength;    // occludedColor = lerp(color, color * <sampled occlusion texture value>, <occlusion strength>)
};
struct Emissive : public Texture<Emissive>
{
    std::vector<double> factor;
};
class Material
{
public:
    //cppcheck-suppress uninitMemberVar
    Material() = default;
    Material(const tinygltf::Model& model, const tinygltf::Material& material);
public:
    [[nodiscard]] MaterialView view_materials() const;
private:
    [[nodiscard]] BaseView view_base() const;
    [[nodiscard]] MetallicRoughnessView view_metal_rough() const;
    [[nodiscard]] NormalView view_normal() const;
    [[nodiscard]] OcclusionView view_occlusion() const;
    [[nodiscard]] EmissiveView view_emissive() const;
private:
    Alpha m_AlphaSettings;
    std::optional<Base> m_Base;
    std::optional<MetallicRoughness> m_MetalRough;
    std::optional<Normal> m_Normal;
    std::optional<Occlusion> m_Occlusion;
    std::optional<Emissive> m_Emissive;
    bool m_DoubleSided;
};
}    // namespace asl
