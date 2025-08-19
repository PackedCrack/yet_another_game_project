//
// Created by qwerty on 19/08/2025.
//
#pragma once

#include "Buffer.hpp"
#include "GeometryBuffer.hpp"
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
typedef uint16_t Index;
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
    using vertex_t = Vertex;
    VertexBuffer(std::uint32_t numElements, AllocatedBuffer buffer, std::function<void(AllocatedBuffer)> deleter);
public:
    void bind(CommandBufferRef cmdBuffer) const;
    [[nodiscard]] std::uint32_t push_back(std::span<const vertex_t> vertices);
    [[nodiscard]] VertexDescription get_vertex_description();
private:
    GeometryBuffer m_InsertTracker;
};
}    // namespace odin::graphics::vk::resource
