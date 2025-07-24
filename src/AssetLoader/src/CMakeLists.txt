SET(SOURCES
    tinygltf_impl.cpp
        tinygltf_impl.hpp
        Model.hpp
        Model.cpp
        Mesh.hpp
        Renderable.cpp
        Renderable.hpp
        Mesh.cpp
        Material.cpp
        Material.hpp
        gltf_loader.hpp
        gltf_loader.cpp
        asl_defines.hpp
)

set(EXPORT_HEADERS
        Model.hpp
)

target_sources(${ASSET_LOADER_PROJECT} PRIVATE ${SOURCES})
copy_export_headers("${EXPORT_HEADERS}" ${INCLUDE_DIR})