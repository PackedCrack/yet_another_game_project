SET(SOURCES
    tinygltf_impl.cpp
    tinygltf_impl.h
    ModelData.h
    ModelDataImpl.h
    ModelDataImpl.cpp
        Mesh.h
        Mesh.cpp
        gltf_loader.h
        gltf_loader.cpp
        asl_defines.h
)

set(EXPORT_HEADERS
        ModelData.h
)

target_sources(${ASSET_LOADER_PROJECT} PRIVATE ${SOURCES})
copy_export_headers("${EXPORT_HEADERS}" ${INCLUDE_DIR})