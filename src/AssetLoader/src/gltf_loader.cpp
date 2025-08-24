#include "gltf_loader.hpp"

#include "asl_defines.hpp"
#include "common.hpp"
#include "SceneGraph.hpp"
// glm
#include <glm_headers.hpp>
//
//
namespace
{
constexpr std::int64_t NONE = -1;
[[nodiscard]] bool has_skin(const tinygltf::Node& node)
{
    return node.skin != NONE;
}
[[nodiscard]] bool has_local_matrix(const tinygltf::Node& node)
{
    return !node.matrix.empty();
}
[[nodiscard]] asl::TRS make_translation_rotation_scale(const tinygltf::Node& node)
{
    static constexpr std::size_t x = 0;
    static constexpr std::size_t y = 1;
    static constexpr std::size_t z = 2;
    static constexpr std::size_t w = 3;

    asl::TRS trs{ .orientation = glm::quat(0.0f, 0.0f, 0.0f, 1.0f), .translation = glm::vec3(0.0f, 0.0f, 0.0f), .scale = 1.0f };
    if (has_local_matrix(node))
    {
        [[maybe_unused]] glm::vec3 skew{};
        [[maybe_unused]] glm::vec4 projection{};

        glm::vec3 scale{};
        glm::vec3 translation{};
        ODIN_ASSERT(false);
        // PROBABLY A BUG SINCE WE'RE NOT CASTING FROM DOUBLE TO FLOAT
        glm::mat4 localMatrix = glm::make_mat4x4(node.matrix.data());
        glm::decompose(localMatrix, scale, trs.orientation, translation, skew, projection);

        trs.translation = glm::vec4(translation, 1.0f);

        ODIN_ASSERT(scale[x] == scale[y]);
        ODIN_ASSERT(scale[y] == scale[z]);
        trs.scale = scale[x];
    }
    else
    {
        if (!node.translation.empty())
        {
            ODIN_ASSERT(node.translation.size() == 3);
            trs.translation = glm::vec3{ static_cast<float>(node.translation[x]),
                                         static_cast<float>(node.translation[y]),
                                         static_cast<float>(node.translation[z]) };
        }
        if (!node.rotation.empty())
        {
            ODIN_ASSERT(node.rotation.size() == 4);
            glm::vec4 asFloat{ static_cast<float>(node.rotation[x]),
                               static_cast<float>(node.rotation[y]),
                               static_cast<float>(node.rotation[z]),
                               static_cast<float>(node.rotation[w]) };
            trs.orientation = glm::make_quat(std::addressof(asFloat[x]));
        }
        if (!node.scale.empty())
        {
            ODIN_ASSERT(node.scale.size() == 3);
            ODIN_ASSERT(node.scale[x] == node.scale[y]);
            ODIN_ASSERT(node.scale[y] == node.scale[z]);
            trs.scale = static_cast<float>(node.scale[x]);
        }
    }

    return trs;
}
[[nodiscard]] asl::SceneGraphNode make_model_node(const tinygltf::Model& model, const tinygltf::Node& node)
{
    // TODO: extensions support goes here
    // root.extensions

    asl::TRS trs = make_translation_rotation_scale(node);
    asl::SceneGraphNode modelNode{ trs, asl::make_mesh(model, node) };

    for (auto&& index : node.children)
    {
        const tinygltf::Node& child = model.nodes[index];
        [[maybe_unused]] asl::SceneGraphNode& m = modelNode.emplace_neighbour(make_model_node(model, child));
    }

    // TODO: weights goes here
    // mesh = extract_weights(root.weights, mesh);


    if (has_skin(node))
    {
        // TODO: skin extraction goes here
        // mesh = extract_skin_data(model, node, mesh);
    }

    return modelNode;
}
[[nodiscard]] asl::SceneGraphNode make_dummy_node()
{
    asl::TRS trs{ .orientation = glm::quat(0.0f, 0.0f, 0.0f, 1.0f), .translation = glm::vec3(0.0f, 0.0f, 0.0f), .scale = 1.0f };
    return asl::SceneGraphNode{ trs, std::nullopt };
}
[[nodiscard]] common::CGraph<asl::SceneGraphNode> make_scene_graph(const tinygltf::Model& model)
{
    if (model.scenes.size() != 1)
    {
        LOG_FATAL("Tried to load a GLTF model with multiple scenes! Expected only 1.");
    }

    const tinygltf::Scene& scene = model.scenes.front();
    if (!scene.name.empty())
    {
        LOG_DEBUG("Found scene: {}", scene.name.c_str());
    }

    // TODO: extensions support goes here
    // scene.extensions

    //asl::ModelNode root{};
    // indices for the root nodes of the scene
    const std::vector<int32_t>& indices = scene.nodes;
    if (indices.size() != 1)
    {
        asl::SceneGraphNode root = make_dummy_node();
        for (auto&& index : indices)
        {
            auto i = static_cast<std::size_t>(index);
            const tinygltf::Node& tinynode = model.nodes[i];
            [[maybe_unused]] asl::SceneGraphNode& m = root.emplace_neighbour(make_model_node(model, tinynode));
        }
        return common::CGraph{ std::move(root) };
    }
    else
    {
        const tinygltf::Node& tinynode = model.nodes[static_cast<std::size_t>(indices.front())];
        //root = make_model_node(model, tinynode);
        return common::CGraph{ make_model_node(model, tinynode) };
    }

    //return common::CGraph{ std::move(root) };
}
[[nodiscard]] common::CGraph<asl::SceneGraphNode> load_glb(const std::filesystem::path& filename)
{
    tinygltf::TinyGLTF loader{};
    std::string warn{};
    std::string err{};
    tinygltf::Model model{};
    if (!loader.LoadBinaryFromFile(&model, &err, &warn, filename.string()))
    {
        // TODO: LOAD ERROR MODEL HERE?
        LOG_FATAL("LOADING::GLB::FILE::ERROR: \"{}\"", err.c_str());
    }
    if (!warn.empty())
    {
        LOG_WARN("LOADING::GLB::FILE::WARNING: \"{}\"", warn.c_str());
    }

    return make_scene_graph(model);
}
}    // namespace
namespace asl
{
common::CGraph<SceneGraphNode> load_model(const std::filesystem::path& filename)
{
    LOG_DEBUG("Loading GLTF file: {}", filename.string().c_str());

    ODIN_ASSERT(filename.has_extension());
    std::string extension = filename.extension().string();
    if (extension == ".gltf")
    {
        LOG_FATAL("NYI::GLTF::FILE::EXTENSION::{}", filename.string().c_str());
        //return load_gltf(filename);
    }
    if (extension == ".glb")
    {
        return load_glb(filename);
    }

    LOG_FATAL("GLTF::UNKNOWN::FILE::EXTENSION::{}", filename.string().c_str());
}
}    // namespace asl
