//
// Created by qwerty on 24/07/2025.
//
#include "Renderable.hpp"

#include "asl_defines.hpp"
#include "../common/common.h"
//
//
namespace
{
constexpr std::int32_t NONE = -1;
// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#_accessor_componenttype
enum class AccessorComponentType
{
    BYTE = TINYGLTF_COMPONENT_TYPE_BYTE,
    UNSIGNED_BYTE = TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE,
    SHORT = TINYGLTF_COMPONENT_TYPE_SHORT,
    UNSIGNED_SHORT = TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT,
    UNSIGNED_INT = TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT,
    FLOAT = TINYGLTF_COMPONENT_TYPE_FLOAT
};
// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#_accessor_type
enum class AccessorType
{
    VEC2 = TINYGLTF_TYPE_VEC2,
    VEC3 = TINYGLTF_TYPE_VEC3,
    VEC4 = TINYGLTF_TYPE_VEC4,
    MAT2 = TINYGLTF_TYPE_MAT2,
    MAT3 = TINYGLTF_TYPE_MAT3,
    MAT4 = TINYGLTF_TYPE_MAT4,
    SCALAR = TINYGLTF_TYPE_SCALAR
};
[[nodiscard]] int32_t component_size(AccessorComponentType componentType)
{
    UNHANDLED_CASE_PROTECTION_ON
    // clang-format off
    switch (componentType)
    {
    case AccessorComponentType::BYTE: return sizeof(int8_t);
    case AccessorComponentType::UNSIGNED_BYTE: return sizeof(uint8_t);
    case AccessorComponentType::SHORT: return sizeof(int16_t);
    case AccessorComponentType::UNSIGNED_SHORT: return sizeof(uint16_t);
    case AccessorComponentType::UNSIGNED_INT:return sizeof(uint32_t);
    case AccessorComponentType::FLOAT: return sizeof(float);
    }
    // clang-format on
    UNHANDLED_CASE_PROTECTION_OFF
    std::unreachable();
}
[[nodiscard]] int32_t num_components(AccessorType type)
{
    UNHANDLED_CASE_PROTECTION_ON
    // clang-format off
    switch (type)
    {
    case AccessorType::SCALAR: return 1;
    case AccessorType::VEC2: return 2;
    case AccessorType::VEC3: return 3;
    case AccessorType::VEC4: [[fallthrough]];
    case AccessorType::MAT2: return 4;
    case AccessorType::MAT3: return 9;
    case AccessorType::MAT4: return 16;
    }
    // clang-format on
    UNHANDLED_CASE_PROTECTION_OFF
    std::unreachable();
}
template<typename element_t>
[[nodiscard]] bool valid_element_type(const tinygltf::Accessor* pAccessor)
{
    // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#meshes-overview
    const int32_t COMPONENT_SIZE = component_size(static_cast<AccessorComponentType>(pAccessor->componentType));
    const int32_t NUM_COMPONENTS = num_components(static_cast<AccessorType>(pAccessor->type));
    const int32_t ELEMENT_SIZE = COMPONENT_SIZE * NUM_COMPONENTS;
    return sizeof(element_t) == ELEMENT_SIZE;
}
[[nodiscard]] bool has_mesh(const tinygltf::Node& node)
{
    return node.mesh != NONE;
}
[[nodiscard]] bool has_mode(const tinygltf::Primitive& primitive)
{
    return primitive.mode != NONE;
}
[[nodiscard]] bool has_material(const tinygltf::Primitive& primitive)
{
    return primitive.material != NONE;
}
[[nodiscard]] bool has_indices(const tinygltf::Primitive& primitive)
{
    return primitive.indices != NONE;
}
[[nodiscard]] const tinygltf::Buffer& get_buffer(const tinygltf::Model& model, std::int32_t bufferIndex)
{
    ODIN_ASSERT(bufferIndex != NONE);
    // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#buffers-and-buffer-views
    return model.buffers[static_cast<std::size_t>(bufferIndex)];
}
template<typename element_t>
[[nodiscard]] std::vector<element_t>
extract_stridefull_buffer(const tinygltf::Accessor* pAccessor, const tinygltf::BufferView& view, const tinygltf::Buffer& buffer)
{
    auto offset = static_cast<int64_t>(pAccessor->byteOffset + view.byteOffset);
    auto attribute = common::make_view<const uint8_t>(buffer.data, view.byteLength, offset);

    ODIN_ASSERT((view.byteLength % view.byteStride) == 0);
    std::vector<element_t> extractedData{};
    for (std::size_t i = 0u; i < view.byteLength; i = i + view.byteStride)
    {
        auto elementView = common::make_view<const uint8_t>(attribute, sizeof(element_t), static_cast<int64_t>(i));
        extractedData.emplace_back();
        std::memcpy(&extractedData.back(), elementView.data(), elementView.size());
    }

    ODIN_ASSERT(extractedData.size() == pAccessor->count);
    return extractedData;
}
template<typename element_t>
[[nodiscard]] std::vector<element_t>
extract_strideless_buffer(const tinygltf::Accessor* pAccessor, const tinygltf::BufferView& view, const tinygltf::Buffer& buffer)
{
    std::size_t numElements = pAccessor->count;
    auto offset = static_cast<int64_t>(pAccessor->byteOffset + view.byteOffset);
    auto attribute = common::make_view<const uint8_t>(buffer.data, view.byteLength, offset);

    std::vector<element_t> extractedData{};
    extractedData.resize(numElements);
    ODIN_ASSERT(numElements * sizeof(element_t) == attribute.size());
    std::memcpy(extractedData.data(), attribute.data(), attribute.size());

    return extractedData;
}
template<typename element_t>
[[nodiscard]] std::vector<element_t> extract_buffer(const tinygltf::Model& model, const tinygltf::Accessor* pAccessor)
{
    // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#accessors
    ODIN_ASSERT(valid_element_type<element_t>(pAccessor));
    ODIN_ASSERT(pAccessor->bufferView != NONE)

    // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#buffers-and-buffer-views
    auto bvIndex = static_cast<std::size_t>(pAccessor->bufferView);
    const tinygltf::BufferView& view = model.bufferViews[bvIndex];
    const tinygltf::Buffer& buffer = get_buffer(model, view.buffer);

    if (view.byteStride == 0)
    {
        return extract_strideless_buffer<element_t>(pAccessor, view, buffer);
    }

    return extract_stridefull_buffer<element_t>(pAccessor, view, buffer);
}
[[nodiscard]] std::optional<const tinygltf::Accessor*>
find_attribute_accessor(const tinygltf::Model& model, const tinygltf::Primitive& primitive, std::string_view attribute)
{
    auto it = primitive.attributes.find(attribute.data());
    if (it == primitive.attributes.end())
    {
        return std::nullopt;
    }

    auto accessor = static_cast<std::size_t>(it->second);
    return std::optional<const tinygltf::Accessor*>{ std::in_place, &model.accessors[accessor] };
}
template<typename accessor_t>
[[nodiscard]] std::optional<std::vector<accessor_t>>
get_attribute(const tinygltf::Model& model, const tinygltf::Primitive& primitive, std::string_view attribute)
{
    // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#meshes-overview
    std::optional<const tinygltf::Accessor*> accessor = find_attribute_accessor(model, primitive, attribute);
    if (!accessor)
    {
        return std::nullopt;
    }

    return std::optional<std::vector<accessor_t>>{ std::in_place, extract_buffer<accessor_t>(model, *accessor) };
}
template<typename vec_t>
void normalize(std::vector<vec_t>& out_data)
{
    std::for_each(std::execution::par, std::begin(out_data), std::end(out_data), [](auto&& vec) { vec = glm::normalize(vec); });
}
template<typename cache_t>
[[nodiscard]] cache_t extract_attributes(const tinygltf::Model& model, const tinygltf::Primitive& primitive, std::string_view attributeName)
{
    using vector_size = typename cache_t::mapped_type::value_type;

    cache_t cache{};
    for (std::int64_t i = 0; i < std::numeric_limits<std::int64_t>::max(); ++i)
    {
        std::string attribute = std::format("{}_{}", attributeName.data(), i);
        auto a = get_attribute<vector_size>(model, primitive, attribute);
        if (!a.has_value())
        {
            break;
        }

        normalize(*a);
        auto [it, emplaced] = cache.try_emplace(attribute, std::move(a.value()));
        ODIN_ASSERT(emplaced);
        LOG_DEBUG("Added attribute set: \"{}\"", attribute.c_str());
    }

    return cache;
}
[[nodiscard]] asl::Renderable::color_cache extract_colors(const tinygltf::Model& model, const tinygltf::Primitive& primitive)
{
    asl::Renderable::color_cache cache{};
    for (std::int64_t i = 0; i < std::numeric_limits<std::int64_t>::max(); ++i)
    {
        std::string attribute = std::format("{}_{}", "COLOR", i);
        std::optional<const tinygltf::Accessor*> accessor = find_attribute_accessor(model, primitive, attribute);
        if (!accessor)
        {
            break;
        }

        const tinygltf::Accessor* pAccessor = *accessor;
        if (AccessorType{ pAccessor->type } == AccessorType::VEC4)
        {
            auto [it, emplaced] = cache.try_emplace(attribute, extract_buffer<glm::vec4>(model, *accessor));
            ODIN_ASSERT(emplaced);
        }
        else
        {
            // convert vec3 to vec4
            ODIN_ASSERT(AccessorType{ pAccessor->type } == AccessorType::VEC3);
            LOG_WARN("Mesh primitive from loaded GLTF file has color data in RGB. Converting to RGBA..");

            std::optional<std::vector<glm::vec3>> color{ std::in_place, extract_buffer<glm::vec3>(model, *accessor) };
            ODIN_ASSERT(color);

            std::vector<glm::vec4> convertedColor{};
            for (auto&& vec3 : *color)
            {
                convertedColor.emplace_back(1.0f);
                convertedColor.back().x = vec3.x;
                convertedColor.back().y = vec3.y;
                convertedColor.back().z = vec3.z;
            }

            auto [it, emplaced] = cache.try_emplace(attribute, std::move(convertedColor));
            ODIN_ASSERT(emplaced);
        }
        LOG_DEBUG("Added attribute set: \"{}\"", attribute.c_str());
    }

    return cache;
}
[[nodiscard]] std::optional<std::vector<glm::vec4>> extract_vertex_tangents(const tinygltf::Model& model,
                                                                            const tinygltf::Primitive& primitive)
{
    using tangents = std::vector<glm::vec4>;
    std::optional<tangents> t = get_attribute<glm::vec4>(model, primitive, "TANGENT");
    if (!t)
    {
        return std::nullopt;
    }

    normalize(*t);
    return std::optional<tangents>{ std::in_place, std::move(t.value()) };
}
[[nodiscard]] std::optional<std::vector<glm::vec3>> extract_vertex_normals(const tinygltf::Model& model,
                                                                           const tinygltf::Primitive& primitive)
{
    std::optional<std::vector<glm::vec3>> normals = get_attribute<glm::vec3>(model, primitive, "NORMAL");
    if (!normals)
    {
        return std::nullopt;
    }

    normalize(*normals);
    return std::optional<std::vector<glm::vec3>>{ std::in_place, std::move(normals.value()) };
}
[[nodiscard]] std::vector<glm::vec4> extract_vertex_positions(const tinygltf::Model& model, const tinygltf::Primitive& primitive)
{
    using vertex_positions = std::vector<glm::vec3>;
    std::optional<vertex_positions> p = get_attribute<glm::vec3>(model, primitive, "POSITION").value();
    std::vector<glm::vec3> positions = std::move(p.value());

    std::vector<glm::vec4> paddedPositions{};
    paddedPositions.resize(positions.size());
    auto action = [](const glm::vec3& v) { return glm::vec4{ v.x, v.y, v.z, 1.0f }; };
    std::transform(std::execution::par, std::begin(positions), std::end(positions), std::begin(paddedPositions), action);

    return paddedPositions;
}
[[nodiscard]] asl::Renderable&
store_attributes(const tinygltf::Model& model, const tinygltf::Primitive& primitive, asl::Renderable& renderable)
{
    renderable.vertexPosition = extract_vertex_positions(model, primitive);
    renderable.normal = extract_vertex_normals(model, primitive);
    renderable.tangent = extract_vertex_tangents(model, primitive);
    renderable.textureCoordinates = extract_attributes<asl::Renderable::uv_cache>(model, primitive, "TEXCOORD");
    renderable.colors = extract_colors(model, primitive);
    renderable.joints = extract_attributes<asl::Renderable::joints_cache>(model, primitive, "JOINTS");
    renderable.weights = extract_attributes<asl::Renderable::weights_cache>(model, primitive, "WEIGHTS");

    return renderable;
}
template<typename element_t>
requires std::integral<element_t>
[[nodiscard]] std::vector<std::uint16_t> convert_buffer_to_u16(const std::vector<element_t>& buffer)
{
    std::vector<uint16_t> converted{};
    converted.resize(buffer.size());
    std::transform(std::execution::par,
                   std::begin(buffer),
                   std::end(buffer),
                   std::begin(converted),
                   [](element_t val) { return static_cast<std::uint16_t>(static_cast<std::make_unsigned_t<element_t>>(val)); });

    return converted;
}
[[nodiscard]] std::vector<std::uint16_t> extract_indices(const tinygltf::Model& model, const tinygltf::Primitive& primitive)
{
    ODIN_ASSERT(primitive.indices != NONE);

    auto index = static_cast<std::size_t>(primitive.indices);
    const tinygltf::Accessor* pAccessor = std::addressof(model.accessors[index]);

    auto type = AccessorComponentType{ pAccessor->componentType };
    switch (type)
    {
    case AccessorComponentType::UNSIGNED_SHORT:
        return extract_buffer<std::uint16_t>(model, pAccessor);
    case AccessorComponentType::UNSIGNED_BYTE:
    {
        LOG_INFO("Indices are uint8.. converting to uint16.");
        return convert_buffer_to_u16(extract_buffer<std::uint8_t>(model, pAccessor));
    }
    case AccessorComponentType::UNSIGNED_INT:
    {
        LOG_WARN("Indices are uint32! Truncating to uint16..");
        return convert_buffer_to_u16(extract_buffer<std::uint32_t>(model, pAccessor));
    }
    default:
        LOG_FATAL("Unexpected component type when extracting indices");
    }

    std::unreachable();
}
}    // namespace
namespace asl
{
std::optional<Mesh> make_mesh(const tinygltf::Model& model, const tinygltf::Node& node)
{
    if (!has_mesh(node))
    {
        return std::nullopt;
    }

    const tinygltf::Mesh& gltfMesh = model.meshes[static_cast<std::size_t>(node.mesh)];
    // A node can contain multiple primitives - but for now implement handling of only 1
    if (gltfMesh.primitives.size() > 1)
    {
        LOG_FATAL("Tried loading a mesh that contains multiple primitives, when this is not supported");
    }
    ODIN_ASSERT(!gltfMesh.primitives.empty());    // Uncertian if this can be empty - if breakpoint hits it needs to be investigated.

    // TODO: extensions support goes here
    // gltfMesh.extensions

    // TODO: weights goes here
    // gltfMesh.weights;

    std::optional<Mesh> mesh{ std::in_place, Mesh{} };
    std::vector<Renderable>& renderables = mesh->renderables;

    const std::vector<tinygltf::Primitive>& primitives = gltfMesh.primitives;
    for (auto&& primitive : primitives)
    {
        renderables.emplace_back();
        Renderable& renderable = renderables.back();

        // TODO: extensions support goes here
        // m.extensions


        renderable = store_attributes(model, primitive, renderable);

        if (has_indices(primitive))
        {
            renderable.indices = extract_indices(model, primitive);
        }

        if (has_material(primitive))
        {
            auto index = static_cast<std::size_t>(primitive.material);
            const tinygltf::Material& material = model.materials[index];
            renderable.material = Material{ model, material };
        }

        if (has_mode(primitive))
        {
            renderable.topology = PrimitiveMode{ primitive.mode };
            if (renderable.topology != PrimitiveMode::TRIANGLES)
            {
                LOG_WARN("Loading a GLTF mesh who's primitive is not set to Triangles!");
            }
        }
    }

    return mesh;
}
}    // namespace asl
