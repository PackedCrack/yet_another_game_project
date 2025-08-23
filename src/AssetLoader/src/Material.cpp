//
// Created by qwerty on 25/07/2024.
//
#include "Material.hpp"
#include "asl_defines.hpp"
#include "common.hpp"
//
//
// Check MinFilter
static_assert(std::to_underlying(asl::MinFilter::nearest) == TINYGLTF_TEXTURE_FILTER_NEAREST);
static_assert(std::to_underlying(asl::MinFilter::linear) == TINYGLTF_TEXTURE_FILTER_LINEAR);
static_assert(std::to_underlying(asl::MinFilter::nearestMipMapNearest) == TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST);
static_assert(std::to_underlying(asl::MinFilter::linearMipMapNearest) == TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST);
static_assert(std::to_underlying(asl::MinFilter::nearestMipMapLinear) == TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR);
static_assert(std::to_underlying(asl::MinFilter::linearMipMapLinear) == TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR);
// Check MagFilter
static_assert(std::to_underlying(asl::MagFilter::nearest) == TINYGLTF_TEXTURE_FILTER_NEAREST);
static_assert(std::to_underlying(asl::MagFilter::linear) == TINYGLTF_TEXTURE_FILTER_LINEAR);
// Check Wrapping
static_assert(std::to_underlying(asl::Wrapping::repeat) == TINYGLTF_TEXTURE_WRAP_REPEAT);
static_assert(std::to_underlying(asl::Wrapping::clampToEdge) == TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE);
static_assert(std::to_underlying(asl::Wrapping::mirroredRepeat) == TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT);
//
//
namespace
{
constexpr std::int32_t NONE = -1;
//
//
[[nodiscard]] const tinygltf::Image& get_image(const tinygltf::Model& model, const tinygltf::Texture& texture)
{
    std::int32_t i = texture.source;
    ODIN_ASSERT(i != NONE);
    return model.images[static_cast<std::size_t>(i)];
}
template<typename texture_info_t>
[[nodiscard]] const tinygltf::Texture& get_texture(const tinygltf::Model& model, const texture_info_t& info)
{
    std::int32_t i = info.index;
    ODIN_ASSERT(i != NONE);
    return model.textures[static_cast<std::size_t>(i)];
}
[[nodiscard]] std::string make_texture_key(std::int32_t uvSetIndex)
{
    return std::format("TEXCOORD_{}", static_cast<std::size_t>(uvSetIndex));
}
template<typename filter_t>
requires common::is_any_of<filter_t, asl::MinFilter, asl::MagFilter>
[[nodiscard]] filter_t to_filter_enum(std::int32_t filter)
{
    if constexpr (std::same_as<filter_t, asl::MinFilter>)
    {
        // clang-format off
        switch (filter)
        // cppcheck-suppress missingReturn
        {
        case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST: return filter_t::nearestMipMapNearest;
        case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST: return filter_t::linearMipMapNearest;
        case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR: return filter_t::nearestMipMapLinear;
        case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR: return filter_t::linearMipMapLinear;
        case TINYGLTF_TEXTURE_FILTER_NEAREST: return filter_t::nearest;
        case TINYGLTF_TEXTURE_FILTER_LINEAR: return filter_t::linear;
        }
        // clang-format on

        LOG_WARN("Unknown MinFilter: {}. Defaulting to Linear.", filter);
        return asl::MinFilter::linear;
    }
    else
    {
        static_assert(std::same_as<filter_t, asl::MagFilter>);

        // clang-format off
        switch (filter)
        {
        case TINYGLTF_TEXTURE_FILTER_NEAREST: return filter_t::nearest;
        case TINYGLTF_TEXTURE_FILTER_LINEAR: return filter_t::linear;
        }
        // clang-format on

        LOG_WARN("Unknown MagFilter: {}. Defaulting to Linear.", filter);
        return asl::MagFilter::linear;
    }
}
[[nodiscard]] asl::Wrapping to_wrap_enum(std::int32_t wrap)
{
    // clang-format off
    switch (wrap)
    {
    case TINYGLTF_TEXTURE_WRAP_REPEAT: return asl::Wrapping::repeat;
    case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE: return asl::Wrapping::clampToEdge;
    case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT: return asl::Wrapping::mirroredRepeat;
    }
    // clang-format on

    LOG_WARN("Unknown Wrap value: {}. Defaulting to repeat.", wrap);
    return asl::Wrapping::repeat;
}
[[nodiscard]] asl::Sampler make_sampler(const tinygltf::Model& model, const tinygltf::Texture& texture)
{
    std::int32_t index = texture.sampler;

    // TODO:
    // Extensions goes here: sampler.extensions

    // cppcheck-suppress knownConditionTrueFalse
    if (index != NONE)
    {
        const tinygltf::Sampler& sampler = model.samplers[static_cast<std::size_t>(index)];

        // clang-format off
        return asl::Sampler{
            .wrapS = to_wrap_enum(sampler.wrapS),
            .wrapT = to_wrap_enum(sampler.wrapT),
            .minfilter = to_filter_enum<asl::MinFilter>(sampler.minFilter),
            .magfilter = to_filter_enum<asl::MagFilter>(sampler.magFilter)
        };
        // clang-format on
    }

    // clang-format off
    return asl::Sampler{
        .wrapS = asl::Wrapping::repeat,
        .wrapT = asl::Wrapping::repeat,
        .minfilter = asl::MinFilter::linear,
        .magfilter = asl::MagFilter::linear
    };
    // clang-format on
}
template<typename texture_t, typename texture_info_t>
requires std::derived_from<texture_t, asl::Texture<texture_t>>
[[nodiscard]] texture_t& make_texture_data(const tinygltf::Model& model, texture_t& outTexture, const texture_info_t& info)
{
    const tinygltf::Texture& texture = get_texture(model, info);
    const tinygltf::Image& image = get_image(model, texture);
    ODIN_ASSERT(!image.image.empty());

    // TODO:
    // Image Extensions goes here: image.extensions
    // TODO:
    // bits goes here: image.bits
    // TODO:
    // pixel_type goes here: image.pixel_type

    if (!common::is_power_of_two(image.width) || !common::is_power_of_two(image.height))
    {
        LOG_WARN(std::format("Texture: {}'s dimensions are not power of two", texture.name));
    }

    outTexture.textureKey = make_texture_key(info.texCoord);
    outTexture.imageBuffer = image.image;
    outTexture.imageWidth = image.width;
    outTexture.imageHeight = image.height;
    outTexture.sampler = make_sampler(model, texture);

    return outTexture;
}
[[nodiscard]] std::optional<asl::Base> make_base_texture(const tinygltf::Model& model, const tinygltf::Material& material)
{
    const tinygltf::TextureInfo& info = material.pbrMetallicRoughness.baseColorTexture;
    if (info.index == NONE)
    {
        return std::nullopt;
    }
    // TODO:
    // TextureInfo extensions goes here: info.extensions

    std::optional<asl::Base> base{ std::in_place, asl::Base{} };
    asl::Base& b = base.value();
    b.factor = material.pbrMetallicRoughness.baseColorFactor;
    b = make_texture_data<asl::Base>(model, b, info);

    return base;
}
[[nodiscard]] std::optional<asl::MetallicRoughness> make_metal_rough_texture(const tinygltf::Model& model,
                                                                             const tinygltf::Material& material)
{
    const tinygltf::TextureInfo& info = material.pbrMetallicRoughness.metallicRoughnessTexture;
    if (info.index == NONE)
    {
        return std::nullopt;
    }
    // TODO:
    // TextureInfo extensions goes here: info.extensions

    std::optional<asl::MetallicRoughness> metalRough{ std::in_place, asl::MetallicRoughness{} };
    asl::MetallicRoughness& mr = metalRough.value();
    mr.metallic = material.pbrMetallicRoughness.metallicFactor;
    mr.roughness = material.pbrMetallicRoughness.roughnessFactor;
    mr = make_texture_data<asl::MetallicRoughness>(model, mr, info);

    return metalRough;
}
[[nodiscard]] std::optional<asl::Normal> make_normal_texture(const tinygltf::Model& model, const tinygltf::Material& material)
{
    const tinygltf::NormalTextureInfo& info = material.normalTexture;
    if (info.index == NONE)
    {
        return std::nullopt;
    }
    // TODO:
    // TextureInfo extensions goes here: info.extensions


    std::optional<asl::Normal> normal{ std::in_place, asl::Normal{} };
    asl::Normal& n = normal.value();
    n.scale = info.scale;
    n = make_texture_data<asl::Normal>(model, n, info);

    return normal;
}
[[nodiscard]] std::optional<asl::Emissive> make_emissive_texture(const tinygltf::Model& model, const tinygltf::Material& material)
{
    const tinygltf::TextureInfo& info = material.emissiveTexture;
    if (info.index == NONE)
    {
        return std::nullopt;
    }
    // TODO:
    // TextureInfo extensions goes here: info.extensions

    std::optional<asl::Emissive> emissive{ std::in_place, asl::Emissive{} };
    asl::Emissive& e = emissive.value();
    e.factor = material.emissiveFactor;
    e = make_texture_data<asl::Emissive>(model, e, info);

    return emissive;
}
[[nodiscard]] std::optional<asl::Occlusion> make_occlusion_texture(const tinygltf::Model& model, const tinygltf::Material& material)
{
    const tinygltf::OcclusionTextureInfo& info = material.occlusionTexture;
    if (info.index == NONE)
    {
        return std::nullopt;
    }

    std::optional<asl::Occlusion> occlusion{ std::in_place, asl::Occlusion{} };
    asl::Occlusion& o = occlusion.value();
    o.strength = info.strength;
    o = make_texture_data<asl::Occlusion>(model, o, info);

    return occlusion;
}
[[nodiscard]] asl::AlphaMode alpha_mode_from_str(std::string_view mode)
{
    if (mode == "OPAQUE")
    {
        return asl::AlphaMode::opaque;
    }
    if (mode == "BLEND")
    {
        return asl::AlphaMode::blend;
    }
    if (mode == "MASK")
    {
        return asl::AlphaMode::mask;
    }
    LOG_FATAL("Expected an alpha mode to exist");
}
[[nodiscard]] asl::Alpha make_alpha_mode(const tinygltf::Material& material)
{
    // clang-format off
    return asl::Alpha{
        .alphaCutoff = material.alphaCutoff,
        .mode = alpha_mode_from_str(material.alphaMode)
    };
    // clang-format on
}
}    // namespace
namespace asl
{
Material::Material(const tinygltf::Model& model, const tinygltf::Material& material)
    : m_AlphaSettings{ make_alpha_mode(material) }
    , m_Base{ make_base_texture(model, material) }
    , m_MetalRough{ make_metal_rough_texture(model, material) }
    , m_Normal{ make_normal_texture(model, material) }
    , m_Occlusion{ make_occlusion_texture(model, material) }
    , m_Emissive{ make_emissive_texture(model, material) }
    , m_DoubleSided{ material.doubleSided }
{
    // TODO:
    // Material extensions goes here: material.extensions
}
MaterialView Material::view_materials() const
{
    MaterialView view{};
    if (m_Base)
    {
        view.base = view_base();
    }
    if (m_MetalRough)
    {
        view.metalRough = view_metal_rough();
    }
    if (m_Normal)
    {
        view.normal = view_normal();
    }
    if (m_Occlusion)
    {
        view.occlusion = view_occlusion();
    }
    if (m_Emissive)
    {
        view.emissive = view_emissive();
    }

    view.alphaSettings = m_AlphaSettings;
    view.doubleSided = m_DoubleSided;

    return view;
}
BaseView Material::view_base() const
{
    BaseView view{};
    view.factor = m_Base->factor;
    view.imageWidth = m_Base->imageWidth;
    view.imageHeight = m_Base->imageHeight;
    view.imageBuffer = std::span<const std::uint8_t>{ m_Base->imageBuffer.data(), m_Base->imageBuffer.size() };
    view.sampler = m_Base->sampler;

    return view;
}
MetallicRoughnessView Material::view_metal_rough() const
{
    MetallicRoughnessView view{};
    view.metallic = m_MetalRough->metallic;
    view.roughness = m_MetalRough->roughness;
    view.imageWidth = m_MetalRough->imageWidth;
    view.imageHeight = m_MetalRough->imageHeight;
    view.imageBuffer = std::span<const std::uint8_t>{ m_MetalRough->imageBuffer.data(), m_MetalRough->imageBuffer.size() };
    view.sampler = m_MetalRough->sampler;

    return view;
}
NormalView Material::view_normal() const
{
    NormalView view{};
    view.scale = m_Normal->scale;
    view.imageWidth = m_Normal->imageWidth;
    view.imageHeight = m_Normal->imageHeight;
    view.imageBuffer = std::span<const std::uint8_t>{ m_Normal->imageBuffer.data(), m_Normal->imageBuffer.size() };
    view.sampler = m_Normal->sampler;

    return view;
}
OcclusionView Material::view_occlusion() const
{
    OcclusionView view{};
    view.strength = m_Occlusion->strength;
    view.imageWidth = m_Occlusion->imageWidth;
    view.imageHeight = m_Occlusion->imageHeight;
    view.imageBuffer = std::span<const std::uint8_t>{ m_Occlusion->imageBuffer.data(), m_Occlusion->imageBuffer.size() };
    view.sampler = m_Occlusion->sampler;

    return view;
}
EmissiveView Material::view_emissive() const
{
    EmissiveView view{};
    view.factor = m_Emissive->factor;
    view.imageWidth = m_Emissive->imageWidth;
    view.imageHeight = m_Emissive->imageHeight;
    view.imageBuffer = std::span<const std::uint8_t>{ m_Emissive->imageBuffer.data(), m_Emissive->imageBuffer.size() };
    view.sampler = m_Emissive->sampler;

    return view;
}
}    // namespace asl
