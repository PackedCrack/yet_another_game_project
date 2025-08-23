find_program(GLSL_VALIDATOR glslangValidator HINTS /usr/bin /usr/local/bin $ENV{VULKAN_SDK}/Bin/ $ENV{VULKAN_SDK}/Bin32/)

# GLSL Sources
file(GLOB_RECURSE GLSL_SOURCE_FILES
    shaders/*.frag
    shaders/*.vert
    shaders/*.comp
)

# Iterate each shader
foreach(GLSL ${GLSL_SOURCE_FILES})
  message(STATUS "BUILDING SHADER")
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/spir-v/${FILE_NAME}.spv")
  message(STATUS ${GLSL})
  # Execute glslang command to compile that specific shader
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)


set(SHADER_TARGET shaders)
add_custom_target(${SHADER_TARGET} SOURCES ${GLSL_SOURCE_FILES} DEPENDS ${SPIRV_BINARY_FILES})


#set_target_properties(${SHADER_TARGET} PROPERTIES FOLDER ${FOLDER_MAIN_PROJECT})