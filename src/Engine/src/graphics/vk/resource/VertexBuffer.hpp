//
// Created by qwerty on 19/08/2025.
//
#pragma once

#include "Buffer.hpp"
#include "../CommandBuffer.hpp"
// glm
#include "glm/glm.hpp"
//
//
namespace odin::graphics::vk::resource
{
struct VertexDescription
{
    std::vector<VkVertexInputBindingDescription> bindings;
    std::vector<VkVertexInputAttributeDescription> attributes;
};
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec4 tangent;
    glm::vec2 texcoord_0;    // Material UV
    glm::vec2 texcoord_1;    // Lightmap/AO
};
class VertexBuffer : public Buffer<VertexBuffer>
{
public:
    [[nodiscard]] static VertexDescription get_vertex_description();
public:
    using vertex_t = Vertex;
    VertexBuffer(AllocatedBuffer buffer, std::function<void(AllocatedBuffer)> deleter);
public:
    void bind(CommandBufferRef cmdBuffer) const;
    [[nodiscard]] std::size_t capacity() const;
};
}    // namespace odin::graphics::vk::resource
