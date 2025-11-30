#pragma once

// glm
#include <glm_headers.hpp>
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
    nearest = 9728,
    linear = 9729,
    nearestMipMapNearest = 9984,
    linearMipMapNearest = 9985,
    nearestMipMapLinear = 9986,
    linearMipMapLinear = 9987
};
enum class MagFilter
{
    nearest = 9728,
    linear = 9729
};
enum class Wrapping
{
    repeat = 10497,
    clampToEdge = 33071,
    mirroredRepeat = 33648
};
struct Sampler
{
    Wrapping wrapS;    // U coordinate
    Wrapping wrapT;    // V coordinate
    std::optional<MinFilter> minfilter;
    std::optional<MagFilter> magfilter;
};
template<typename derived_t>
struct TextureView
{
    std::span<const uint8_t> imageBuffer;
    int32_t imageWidth;
    int32_t imageHeight;
    Sampler sampler;
};
struct BaseView : public TextureView<BaseView>
{
    std::span<const double> factor;
};
struct MetallicRoughnessView : public TextureView<MetallicRoughnessView>
{
    double metallic;
    double roughness;
};
struct NormalView : public TextureView<NormalView>
{
    double scale;
};
struct OcclusionView : public TextureView<OcclusionView>
{
    double strength;
};
struct EmissiveView : public TextureView<EmissiveView>
{
    std::span<const double> factor;
};
struct MaterialView
{
    Alpha alphaSettings;
    std::optional<BaseView> base;
    std::optional<MetallicRoughnessView> metalRough;
    std::optional<NormalView> normal;
    std::optional<OcclusionView> occlusion;
    std::optional<EmissiveView> emissive;
    bool doubleSided;
};
enum class PrimitiveMode
{
    POINTS,
    LINE,
    LINE_LOOP,
    LINE_STRIP,
    TRIANGLES,
    TRIANGLE_STRIP,
    TRIANGLE_FAN
};
struct RenderableView
{
    PrimitiveMode topology;
    std::span<const glm::vec3> vertexPosition;
    std::span<const std::uint16_t> indices;
    std::optional<std::span<const glm::vec3>> normal;
    std::optional<std::span<const glm::vec4>> tangent;
    std::optional<std::span<const glm::vec2>> texcoord_0;
    std::optional<std::span<const glm::vec2>> texcoord_1;
    MaterialView materialView;
};
struct MeshView
{
    std::string_view name;
    std::vector<RenderableView> renderables;
};
struct TRS
{
    glm::quat orientation;
    glm::vec3 translation;
    float scale;
};
struct NodeView
{
    TRS local;
    std::optional<MeshView> geometry;
};
}    // namespace asl
