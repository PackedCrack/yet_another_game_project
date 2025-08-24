SET(SOURCES
    tinygltf_impl.cpp
        tinygltf_impl.hpp
        SceneGraph.hpp
        SceneGraph.cpp
        Mesh.hpp
        Mesh.cpp
        SceneGraphNode.hpp
        SceneGraphNode.cpp
        Material.cpp
        Material.hpp
        gltf_loader.hpp
        gltf_loader.cpp
        AssetRegistry.hpp
        AssetRegistry.cpp
        ModelHandle.hpp
        ModelHandle.cpp
        asl_defines.hpp
)

set(EXPORT_HEADERS
	common_types.hpp
        AssetRegistry.hpp
        ModelHandle.hpp
	SceneGraph.hpp
)

target_sources(${ASSET_LOADER_PROJECT} PRIVATE ${SOURCES})
copy_export_headers("${EXPORT_HEADERS}" ${INCLUDE_DIR})