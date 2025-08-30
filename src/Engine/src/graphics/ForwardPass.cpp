//
// Created by qwerty on 26/07/2025.
//
#include "ForwardPass.hpp"

#include "gpu_types.hpp"
#include "registry/pipeline/Request.hpp"
#include "registry/pipeline/RequestBuilder.hpp"
#include "registry/resource/shader/ShaderHandle.hpp"
#include "vk/pipeline/DescriptorWriter.hpp"
//
//
namespace
{
using namespace odin::graphics;
//
//
[[nodiscard]] registry::pipeline::Request make_request(registry::resource::ResourceRegistry& resourceRegistry)
{
    using ShaderStage = registry::pipeline::ShaderStage;
    using ShaderHandle = registry::resource::shader::ShaderHandle;
    using DescriptorType = registry::pipeline::DescriptorType;
    using ShaderStage = registry::pipeline::ShaderStage;


    ShaderHandle vert = resourceRegistry.shader("forward_pass.vert");
    ShaderHandle frag = resourceRegistry.shader("forward_pass.frag");

    registry::pipeline::RequestBuilder builder{};
    builder
        .add_descriptor_layout(GLOBAL_SET_ID,
                               GLOBAL_SET_BIND_ID_CAMERA_DATA,
                               DescriptorType::dynamicUniformBuffer,
                               false,
                               ShaderStage::vertex,
                               ShaderStage::fragment)
        .add_color_format(VK_FORMAT_R8G8B8A8_SRGB)    // get this from swapchain's color attachment
        .add_polygon_mode(VK_POLYGON_MODE_FILL);

    // Cppcheck thinks we're using using vert/frag after move if these calls are chained..
    builder.add_vertex_shader(std::move(vert));
    builder.add_fragment_shader(std::move(frag));

    return builder.build();
}
}    // namespace
namespace odin::graphics
{
ForwardPass::ForwardPass(const std::shared_ptr<vk::Allocator>& pAllocator,
                         vk::DeviceRef device,
                         registry::pipeline::PipelineRegistry& pipelineRegistry,
                         registry::resource::ResourceRegistry& resourceRegistry)
    : m_GraphicsRequest{ make_request(resourceRegistry) }
    , m_Pipeline{ pipelineRegistry.graphics_pipeline(m_GraphicsRequest) }
    , m_Set{ pipelineRegistry.allocate_descriptor_set(m_Pipeline, GLOBAL_SET_ID) }
    , m_CameraBuffer{ pAllocator->create_dynamic_uniform_buffer(sizeof(CameraInfo), 3) }
// frames in flight is 3 and this is not final code
{
    vk::pipeline::DescriptorWriter writer{ device, m_Set };
    writer.add_dynamic_uniform_buffer(GLOBAL_SET_BIND_ID_CAMERA_DATA, m_CameraBuffer);
    writer.write_descriptor_set();
}
void ForwardPass::execute(vk::CommandBufferRef cmdBuffer, const VkRenderingInfo& info)
{
    vkCmdBeginRendering(cmdBuffer.handle, std::addressof(info));

    m_Pipeline.acquire()->pipeline.bind(cmdBuffer);

    glm::vec3 camPos(0.0f, 0.0f, 3.0f);
    glm::vec3 target(0.0f);
    glm::vec3 front(0.0f, 0.0f, 1.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);

    glm::vec3 direction = glm::normalize(target - camPos);
    glm::vec3 right = glm::normalize(glm::cross(up, direction));
    up = glm::cross(direction, right);

    std::vector<CameraInfo> c{};
    c.emplace_back();
    CameraInfo& camera = c.back();
    camera.proj = glm::perspective(glm::radians(70.0f), 16.9f, 0.1f, 200.0f);
    camera.view = glm::lookAt(camPos, camPos + front, up);
    camera.viewproj = camera.proj * camera.view;

    static std::uint64_t frameCounter{};
    m_CameraBuffer.write<CameraInfo>(c, frameCounter++);

    vkCmdEndRendering(cmdBuffer.handle);
}
}    // namespace odin::graphics
