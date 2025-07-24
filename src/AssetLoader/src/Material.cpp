//
// Created by qwerty on 25/07/2024.
//
#include "Material.h"
#include "asl_defines.h"
#include "common.h"
//
//
namespace
{
// Helper type to keep track of tinygltf indices easier
struct Index
{
    static constexpr int32_t INVALID = -1;
    // cppcheck-suppress noExplicitConstructor
    Index(int32_t value)
        : value{ value }
    {}
    operator bool() const { return value != INVALID; }
    operator std::size_t() const
    {
        ODIN_ASSERT(value != INVALID);
        return static_cast<std::size_t>(value);
    }
    int32_t value = INVALID;
};
//
template<typename texture_info_t>
concept texture_info = requires(texture_info_t info) {
    std::assignable_from<Index, decltype(info.index)>;
    std::assignable_from<Index, decltype(info.texCoord)>;
};
//
//
[[nodiscard]] const tinygltf::Image& get_image(const tinygltf::Model& model, const tinygltf::Texture& texture)
{
    Index i = texture.source;
    ODIN_ASSERT(i);
    return model.images[i];
}
template<typename texture_info_t>
requires texture_info<texture_info_t>
[[nodiscard]] const tinygltf::Texture& get_texture(const tinygltf::Model& model, const texture_info_t& info)
{
    Index i = info.index;
    ODIN_ASSERT(i);
    return model.textures[i];
}
[[nodiscard]] std::string make_texture_key(Index uvSet)
{
    return std::format("TEXCOORD_{}", static_cast<std::size_t>(uvSet));
}
template<typename filter_t>
requires common::is_any_of<filter_t, asl::MinFilter, asl::MagFilter>
[[nodiscard]] filter_t to_filter_enum(int32_t filter)
{
    UNHANDLED_CASE_PROTECTION_ON
    // clang-format off
    switch (filter)
    {
    case TINYGLTF_TEXTURE_FILTER_NEAREST: return filter_t::nearest;
    case TINYGLTF_TEXTURE_FILTER_LINEAR: return filter_t::linear;
    }
    // clang-format on
    UNHANDLED_CASE_PROTECTION_OFF

    if constexpr (std::same_as<filter_t, asl::MinFilter>)
    {
        UNHANDLED_CASE_PROTECTION_ON
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
        UNHANDLED_CASE_PROTECTION_OFF
    }
    else
    {
        static_assert(std::same_as<filter_t, asl::MagFilter>);
    }

    std::unreachable();
}
[[nodiscard]] asl::Wrapping to_wrap_enum(int32_t wrap)
{
    UNHANDLED_CASE_PROTECTION_ON
    // clang-format off
    switch (wrap)
    {
    case TINYGLTF_TEXTURE_WRAP_REPEAT: return asl::Wrapping::repeat;
    case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE: return asl::Wrapping::clampToEdge;
    case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT: return asl::Wrapping::mirroredRepeat;
    }
    // clang-format on
    UNHANDLED_CASE_PROTECTION_OFF
    std::unreachable();
}
[[nodiscard]] asl::Sampler make_sampler(const tinygltf::Model& model, const tinygltf::Texture& texture)
{
    Index i = texture.sampler;

    // TODO:
    // Extensions goes here: sampler.extensions

    // cppcheck-suppress knownConditionTrueFalse
    if (!i)
    {
        // clang-format off
        return asl::Sampler{
            .wrapS = asl::Wrapping::repeat,
            .wrapT = asl::Wrapping::repeat,
            .minfilter = asl::MinFilter::linear,
            .magfilter = asl::MagFilter::linear
        };
        // clang-format on
    }
    else
    {
        const tinygltf::Sampler& sampler = model.samplers[i];

        // clang-format off
        return asl::Sampler{
            .wrapS = to_wrap_enum(sampler.wrapS),
            .wrapT = to_wrap_enum(sampler.wrapT),
            .minfilter = to_filter_enum<asl::MinFilter>(sampler.minFilter),
            .magfilter = to_filter_enum<asl::MagFilter>(sampler.magFilter)
        };
        // clang-format on
    }
}
template<typename texture_info_t>
requires texture_info<texture_info_t>
[[nodiscard]] std::optional<asl::TextureData> make_texture_data(const tinygltf::Model& model, const texture_info_t& info)
{
    Index i = info.index;
    if (i)
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

        return std::optional<asl::TextureData>{
            std::in_place,
            asl::TextureData{ .texturKey = make_texture_key(Index{ info.texCoord }),
                             .imageBuffer = image.image,
                             .imageWidth = image.width,
                             .imageHeight = image.height,
                             .sampler = make_sampler(model, texture) }
        };
    }
    else
    {
        return std::nullopt;
    }
}
[[nodiscard]] asl::BaseTexture make_base_texture(const tinygltf::Model& model, const tinygltf::Material& material)
{
    const tinygltf::TextureInfo& info = material.pbrMetallicRoughness.baseColorTexture;
    // TODO:
    // TextureInfo extensions goes here: info.extensions

    // clang-format off
    return asl::BaseTexture{
        .factor = material.pbrMetallicRoughness.baseColorFactor,
        .texture = make_texture_data(model, info)
    };
    // clang-format on
}
[[nodiscard]] asl::MetallicRoughnessTexture make_metal_rough_texture(const tinygltf::Model& model, const tinygltf::Material& material)
{
    const tinygltf::TextureInfo& info = material.pbrMetallicRoughness.metallicRoughnessTexture;
    // TODO:
    // TextureInfo extensions goes here: info.extensions

    // clang-format off
    return asl::MetallicRoughnessTexture{
        .metallic = material.pbrMetallicRoughness.metallicFactor,
        .roughness = material.pbrMetallicRoughness.roughnessFactor,
        .texture = make_texture_data(model, info)
    };
    // clang-format on
}
[[nodiscard]] asl::NormalTexture make_normal_texture(const tinygltf::Model& model, const tinygltf::Material& material)
{
    const tinygltf::NormalTextureInfo& info = material.normalTexture;
    // TODO:
    // TextureInfo extensions goes here: info.extensions

    // clang-format off
    return asl::NormalTexture{
        .scale = info.scale,
        .texture = make_texture_data(model, info)
    };
    // clang-format on
}
[[nodiscard]] asl::EmissiveTexture make_emissive_texture(const tinygltf::Model& model, const tinygltf::Material& material)
{
    const tinygltf::TextureInfo& info = material.emissiveTexture;
    // TODO:
    // TextureInfo extensions goes here: info.extensions

    // clang-format off
    return asl::EmissiveTexture{
        .factor = material.emissiveFactor,
        .texture = make_texture_data(model, info)
    };
    // clang-format on
}
[[nodiscard]] asl::OcclusionTexture make_occlusion_texture(const tinygltf::Model& model, const tinygltf::Material& material)
{
    const tinygltf::OcclusionTextureInfo& info = material.occlusionTexture;

    // clang-format off
    return asl::OcclusionTexture{
        .strength = info.strength,
        .texture = make_texture_data(model, info)
    };
    // clang-format on
}
[[nodiscard]] asl::AlphaMode alpha_mode_from_str(std::string_view mode)
{
    if (mode == "OPAQUE")
    {
        return asl::AlphaMode::opaque;
    }
    else if (mode == "BLEND")
    {
        return asl::AlphaMode::blend;
    }
    else if (mode == "MASK")
    {
        return asl::AlphaMode::mask;
    }
    else
    {
        ODIN_ASSERT(false);    // Expecting an alpha mode to exist
        return asl::AlphaMode::opaque;
    }
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
}    // namespace asl
