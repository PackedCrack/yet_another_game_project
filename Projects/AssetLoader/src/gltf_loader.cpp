#include "gltf_loader.h"

#include <cassert>
//
//
namespace asl
{
common::CGraph<Mesh> load_model(const std::filesystem::path& filename)
{
    //const std::string fullpath = assets::FOLDER_PATH.data() + filename;
    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF loader;

    std::string extension = filename.extension().string();
    assert(!extension.empty());

    //std::string warn;
    // std::string err;
    if (extension == ".gltf")
    {
        //ODIN_CHECK(loader.LoadASCIIFromFile(&gltfModel, &err, &warn, fullpath.string()));
    }
    else if (extension == ".glb")
    {
        //ODIN_CHECK(loader.LoadBinaryFromFile(&gltfModel, &err, &warn, fullpath.string()));
    }
    else
    {
        // TODO:: handle unknown extension
        //ODIN_ERROR("GLTF::UNKNOWN::FILE::EXTENSION");
    }
    //log_messages(warn, err);

    return common::CGraph<Mesh>{};
}
}    // namespace asl
