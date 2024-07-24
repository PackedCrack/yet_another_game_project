#include "gltf_loader.h"

#include <cassert>
#include "asl_defines.h"
#include "common.h"
//
//
// void load_meshes(tinygltf::Model& gltfModel, tinygltf::Node& gltfNode, MeshData& meshData)
// {
//     const int32_t MESH_INDEX = gltfNode.mesh;
//     auto& mesh = gltfModel.meshes[MESH_INDEX];

//     for (const auto& primitive : primitives)
//     {
//         // TODO:: Handle other primtive modes
//         PrimitiveMode mode = primitive_mode(primitive.mode);
//         if (mode != PrimitiveMode::TRIANGLES)
//         {
//             ODIN_ERROR_SEVERE("ERROR::PRIMITIVE::MODE::NOT::TRIANGLES");
//         }
//
//
//         int32_t accessorIndex = -1;
//         if (find_attribute_accessor(primitive, "POSITION", accessorIndex))
//         {
//             extract_buffer_data(gltfModel, accessorIndex, meshData.position);
//         }
//         else
//         {
//             ODIN_ERROR_SEVERE("ERROR::PRIMITIVE::HAS::NO::VERTEX::POSITIONS");
//         }
//         // Store the number of vertices..
//         meshData.numVertices = static_cast<uint32_t>(meshData.position.size());
//
//         if (find_attribute_accessor(primitive, "NORMAL", accessorIndex))
//         {
//             extract_buffer_data(gltfModel, accessorIndex, meshData.normal);
//         }
//         else
//         {
//             // If the file doesnt contain normal data we just fill it with 0s
//             meshData.normal.resize(meshData.numVertices);
//         }
//
//         if (find_attribute_accessor(primitive, "TEXCOORD_0", accessorIndex))
//         {
//             extract_buffer_data(gltfModel, accessorIndex, meshData.uv);
//         }
//         else
//         {
//             meshData.uv.resize(meshData.numVertices);
//         }
//
//
//         const int32_t ACCESSOR_INDICES = primitive.indices;
//         load_indices(gltfModel, ACCESSOR_INDICES, meshData);
//
//         const int32_t MATERIAL_INDEX = primitive.material;
//         load_materials(gltfModel, MATERIAL_INDEX, meshData);
//     }
// }
namespace
{
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

typedef std::size_t Index;
constexpr Index HAS_NONE = UINT64_MAX;    // tinygltf set's the indices to -1 if it does not exist.
[[nodiscard]] bool has_material(const tinygltf::Primitive& primitive)
{
    return static_cast<Index>(primitive.material) != HAS_NONE;
}
[[nodiscard]] bool has_indices(const tinygltf::Primitive& primitive)
{
    return static_cast<Index>(primitive.indices) != HAS_NONE;
}
[[nodiscard]] bool has_mode(const tinygltf::Primitive& primitive)
{
    return static_cast<Index>(primitive.mode) != HAS_NONE;
}
[[nodiscard]] bool has_mesh(const tinygltf::Node& node)
{
    if (static_cast<Index>(node.mesh) == HAS_NONE)
    {
        LOG_WARN("GLTF node does not contain a mesh.");
        return false;
    }

    return true;
}
[[nodiscard]] bool has_skin(const tinygltf::Node& node)
{
    return static_cast<Index>(node.skin) != HAS_NONE;
}
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
template<typename vec_t>
void normalize(std::vector<vec_t>& out_data)
{
    for (auto&& vec : out_data)
    {
        vec = glm::normalize(vec);
    }
}
[[nodiscard]] const tinygltf::Buffer& get_buffer(const tinygltf::Model& model, Index buffer)
{
    ODIN_ASSERT(buffer != HAS_NONE);
    // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#buffers-and-buffer-views
    return model.buffers[buffer];
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
template<typename element_t>
[[nodiscard]] std::vector<element_t> extract_buffer(const tinygltf::Model& model, const tinygltf::Accessor* pAccessor)
{
    // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#accessors
    ODIN_ASSERT(valid_element_type<element_t>(pAccessor));
    ODIN_ASSERT(static_cast<Index>(pAccessor->bufferView) != HAS_NONE)    // Expecting a buffer view to exist

    // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#buffers-and-buffer-views
    auto bv = static_cast<Index>(pAccessor->bufferView);
    const tinygltf::BufferView& view = model.bufferViews[bv];
    const tinygltf::Buffer& buffer = get_buffer(model, static_cast<Index>(view.buffer));

    std::size_t numElements = pAccessor->count;
    auto offset = static_cast<int64_t>(pAccessor->byteOffset + view.byteOffset);
    auto attribute = common::make_view<const uint8_t>(buffer.data, view.byteLength, offset);

    if (view.byteStride == 0)
    {
        std::vector<element_t> data{};
        data.resize(numElements);

        std::memcpy(data.data(), attribute.data(), attribute.size());

        return data;
    }
    else
    {
        std::vector<element_t> data{};

        ODIN_ASSERT((view.byteLength % view.byteStride) == 0);
        for (std::size_t i = 0u; i < view.byteLength; i = i + view.byteStride)
        {
            auto elementView = common::make_view<const uint8_t>(attribute, sizeof(element_t), static_cast<int64_t>(i));

            data.emplace_back();
            std::memcpy(&data.back(), elementView.data(), elementView.size());
        }
        ODIN_ASSERT(data.size() == numElements);

        return data;
    }
}
[[nodiscard]] std::optional<const tinygltf::Accessor*>
    find_attribute_accessor(const tinygltf::Model& model, const tinygltf::Primitive& primitive, std::string_view attribute)
{
    auto it = primitive.attributes.find(attribute.data());
    if (it != primitive.attributes.end())
    {
        auto accessor = static_cast<Index>(it->second);
        return std::optional<const tinygltf::Accessor*>{ std::in_place, &model.accessors[accessor] };
    }

    return std::nullopt;
}
template<typename accessor_t>
[[nodiscard]] std::optional<std::vector<accessor_t>>
    get_attribute(const tinygltf::Model& model, const tinygltf::Primitive& primitive, std::string_view attribute)
{
    // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#meshes-overview
    std::optional<const tinygltf::Accessor*> accessor = find_attribute_accessor(model, primitive, attribute);
    if (accessor)
    {
        return std::optional<std::vector<accessor_t>>{ std::in_place, extract_buffer<accessor_t>(model, *accessor) };
    }

    return std::nullopt;
}
[[nodiscard]] std::optional<std::vector<glm::vec4>> get_color_attribute(const tinygltf::Model& model, const tinygltf::Primitive& primitive)
{
    // Investigate models with multiple color sets.
    ODIN_ASSERT(find_attribute_accessor(model, primitive, "COLOR_1") == std::nullopt);

    // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#meshes-overview
    std::optional<const tinygltf::Accessor*> accessor = find_attribute_accessor(model, primitive, "COLOR_0");
    if (!accessor)
    {
        return std::nullopt;
    }

    const tinygltf::Accessor* pAccessor = *accessor;
    if (AccessorType{ pAccessor->type } == AccessorType::VEC4)
    {
        return std::optional<std::vector<glm::vec4>>{ std::in_place, extract_buffer<glm::vec4>(model, *accessor) };
    }
    else
    {
        ODIN_ASSERT(AccessorType{ pAccessor->type } == AccessorType::VEC3);
        LOG_WARN("Mesh primitive from loaded GLTF file has color data in RGB. Converting to RGBA..");

        std::optional<std::vector<glm::vec3>> color{ std::in_place, extract_buffer<glm::vec3>(model, *accessor) };
        ODIN_ASSERT(color);

        std::optional<std::vector<glm::vec4>> convertedColor{ std::in_place, std::vector<glm::vec4>{} };
        for (auto&& vec3 : *color)
        {
            convertedColor->emplace_back(1.0f);
            convertedColor->back().x = vec3.x;
            convertedColor->back().y = vec3.y;
            convertedColor->back().z = vec3.z;
        }

        return convertedColor;
    }
}
[[nodiscard]] asl::Mesh& store_attributes(const tinygltf::Model& model, const tinygltf::Primitive primitive, asl::Mesh& mesh)
{
    std::optional<std::vector<glm::vec3>> position = get_attribute<glm::vec3>(model, primitive, "POSITION");
    ODIN_ASSERT(position);    // vertex positions are expected to exist
    mesh.position = std::move(*position);

    mesh.normal = get_attribute<glm::vec3>(model, primitive, "NORMAL");
    if (mesh.normal)
    {
        normalize(*mesh.normal);
    }

    mesh.tanget = get_attribute<glm::vec4>(model, primitive, "TANGET");
    if (mesh.tanget)
    {
        normalize(*mesh.tanget);
    }

    mesh.uv = get_attribute<glm::vec2>(model, primitive, "TEXCOORD_0");
    mesh.color = get_color_attribute(model, primitive);
    mesh.joints = get_attribute<glm::vec4>(model, primitive, "JOINTS_0");
    mesh.weights = get_attribute<glm::vec4>(model, primitive, "WEIGHTS_0");

    // Investigate models with multiple UV sets.
    ODIN_ASSERT(get_attribute<glm::vec2>(model, primitive, "TEXCOORD_1") == std::nullopt);
    // Investigate models with multiple joints sets.
    ODIN_ASSERT(get_attribute<glm::vec4>(model, primitive, "JOINTS_1") == std::nullopt);
    // Investigate models with multiple weights sets.
    ODIN_ASSERT(get_attribute<glm::vec4>(model, primitive, "WEIGHTS_1") == std::nullopt);

    return mesh;
}
[[nodiscard]] asl::Mesh& extract_primitivtes(const tinygltf::Model& model, const tinygltf::Mesh& m, asl::Mesh& mesh)
{
    const std::vector<tinygltf::Primitive>& primitives = m.primitives;
    for (auto&& primitive : primitives)
    {
        // TODO: extensions support goes here
        // m.extensions

        mesh = store_attributes(model, primitive, mesh);

        if (has_material(primitive))
        {}

        if (has_indices(primitive))
        {}

        if (has_mode(primitive))
        {}
    }

    return mesh;
}
[[nodiscard]] asl::Mesh& extract_mesh_data(const tinygltf::Model& model, const tinygltf::Node& node, asl::Mesh& mesh)
{
    const tinygltf::Mesh& m = model.meshes[static_cast<std::size_t>(node.mesh)];
    // TODO: extensions support goes here
    // m.extensions

    // TODO: weights goes here
    // m.weights;

    // A node can contain multiple primitives - but for now implement handling of only 1
    if (m.primitives.size() > 1)
    {
        LOG_FATAL("Tried loading a mesh that contains multiple primitives, when this is not supported");
    }
    ODIN_ASSERT(!m.primitives.empty());    // Uncertian if this can be empty - if breakpoint hits it needs to be investigated.

    return extract_primitivtes(model, m, mesh);
}
[[nodiscard]] asl::Mesh& store_translation_rotaion_scale(const tinygltf::Node& node, asl::Mesh& mesh)
{
    if (!node.matrix.empty())
    {
        glm::mat4 localMatrix = glm::make_mat4x4(node.matrix.data());

        [[maybe_unused]] glm::vec3 skew{};
        [[maybe_unused]] glm::vec4 projection{};
        glm::decompose(localMatrix, mesh.scale, mesh.rotation, mesh.translation, skew, projection);
    }
    else
    {
        if (!node.translation.empty())
        {
            ODIN_ASSERT(node.translation.size() == 3);
            mesh.translation = glm::make_vec3(node.translation.data());
        }
        if (!node.rotation.empty())
        {
            ODIN_ASSERT(node.rotation.size() == 4);
            mesh.rotation = glm::make_quat(&node.rotation[0]);
        }
        if (!node.scale.empty())
        {
            ODIN_ASSERT(node.scale.size() == 3);
            mesh.scale = glm::make_vec3(node.scale.data());
        }
    }

    return mesh;
}
[[nodiscard]] asl::Mesh extract_node_data(const tinygltf::Model& model, const tinygltf::Node& node)
{
    asl::Mesh mesh{};
    // TODO: extensions support goes here
    // root.extensions

    mesh = store_translation_rotaion_scale(node, mesh);

    // TODO: weights goes here
    // mesh = extract_weights(root.weights, mesh);

    // asl::Mesh is a Vertex in the Model Graph that is generated
    // tinygltf::Mesh refers to a subset of data inside a GLTF Node
    if (has_mesh(node))
    {
        mesh = extract_mesh_data(model, node, mesh);
    }

    if (has_skin(node))
    {
        // TODO: skin extraction goes here
        // mesh = extract_skin_data(model, node, mesh);
    }

    return mesh;
}
[[nodiscard]] const tinygltf::Node& get_root_node(const tinygltf::Model& model)
{
    if (model.scenes.size() != 1)
    {
        LOG_FATAL("Tried to load a GLTF model with multiple scenes! Expected only 1.");
    }

    const tinygltf::Scene& scene = model.scenes.front();
    if (!scene.name.empty())
    {
        LOG_INFO(std::format("Found scene: {}", scene.name));
    }

    // TODO: extensions support goes here
    // scene.extensions

    // indices for the root nodes of the scene
    const std::vector<int32_t>& indices = scene.nodes;
    if (indices.size() != 1)
    {
        LOG_FATAL("GLTF file contains multiple root nodes. The file likely contains multiple models. Investigate");
    }

    return model.nodes[static_cast<std::size_t>(indices.front())];
}
[[nodiscard]] common::CGraph<asl::Mesh> load_glb(const std::filesystem::path& filename)
{
    tinygltf::TinyGLTF loader;

    std::string warn;
    std::string err;
    tinygltf::Model model;
    ODIN_CHECK(loader.LoadBinaryFromFile(&model, &err, &warn, filename.string()));


    const tinygltf::Node& root = get_root_node(model);


    asl::Mesh mesh = extract_node_data(model, root);


    return common::CGraph<asl::Mesh>{};
}
[[nodiscard]] common::CGraph<asl::Mesh> load_gltf(const std::filesystem::path& filename)
{
    tinygltf::TinyGLTF loader;

    std::string warn;
    std::string err;
    tinygltf::Model model;
    ODIN_CHECK(loader.LoadASCIIFromFile(&model, &err, &warn, filename.string()));

    return common::CGraph<asl::Mesh>{};
}
}    // namespace
namespace asl
{
common::CGraph<Mesh> load_model(const std::filesystem::path& filename)
{
    ODIN_ASSERT(filename.has_extension());
    std::string extension = filename.extension().string();

    if (extension == ".gltf")
    {
        LOG_INFO(std::format("Loading model: {}", filename.string()));
        return load_gltf(filename);
    }
    else if (extension == ".glb")
    {
        return load_glb(filename);
    }
    else
    {
        LOG_ERR("GLTF::UNKNOWN::FILE::EXTENSION");
        ODIN_ASSERT(false);

        return common::CGraph<Mesh>{};
    }
}
}    // namespace asl
