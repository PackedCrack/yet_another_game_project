//
// Created by qwerty on 19/08/2025.
//
#include "VertexBuffer.hpp"
//
//
namespace
{
using namespace odin::graphics::vk::resource;
//
//
[[nodiscard]] VkVertexInputAttributeDescription texcoord_1_attribute(std::uint32_t binding, std::uint32_t location)
{
    return VkVertexInputAttributeDescription{ .location = location,
                                              .binding = binding,
                                              .format = VK_FORMAT_R32G32_SFLOAT,
                                              .offset = offsetof(Vertex, texcoord_1) };
}
[[nodiscard]] VkVertexInputAttributeDescription texcoord_0_attribute(std::uint32_t binding, std::uint32_t location)
{
    VkVertexInputAttributeDescription attribute = texcoord_1_attribute(binding, location);
    attribute.offset = offsetof(Vertex, texcoord_0);
    return attribute;
}
[[nodiscard]] VkVertexInputAttributeDescription tangent_attribute(std::uint32_t binding, std::uint32_t location)
{
    return VkVertexInputAttributeDescription{ .location = location,
                                              .binding = binding,
                                              .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                                              .offset = offsetof(Vertex, tangent) };
}
[[nodiscard]] VkVertexInputAttributeDescription normal_attribute(std::uint32_t binding, std::uint32_t location)
{
    return VkVertexInputAttributeDescription{ .location = location,
                                              .binding = binding,
                                              .format = VK_FORMAT_R32G32B32_SFLOAT,
                                              .offset = offsetof(Vertex, normal) };
}
[[nodiscard]] VkVertexInputAttributeDescription position_attribute(std::uint32_t binding, std::uint32_t location)
{
    return VkVertexInputAttributeDescription{ .location = location,
                                              .binding = binding,
                                              .format = VK_FORMAT_R32G32B32_SFLOAT,
                                              .offset = offsetof(Vertex, position) };
}
}    // namespace
namespace odin::graphics::vk::resource
{
VertexBuffer::VertexBuffer(std::uint32_t numElements, AllocatedBuffer buffer, std::function<void(AllocatedBuffer)> deleter)
    : Buffer<VertexBuffer>{ buffer, std::move(deleter) }
    , m_InsertTracker{ numElements * static_cast<std::uint32_t>(sizeof(vertex_t)) }
{}
void VertexBuffer::bind(CommandBufferRef cmdBuffer) const
{
    BufferRef buffer = handle();
    vkCmdBindVertexBuffers(cmdBuffer.handle, 0u, 1u, std::addressof(buffer.handle), nullptr /*?*/);
}
std::uint32_t VertexBuffer::push_back(std::span<const vertex_t> vertices)
{
    std::uint32_t bufferOffset = m_InsertTracker.queue_transfer(vertices);
    std::uint32_t vertexOffset = static_cast<uint32_t>(bufferOffset / sizeof vertex_t);
    return vertexOffset;
}
VertexDescription VertexBuffer::get_vertex_description()
{
    VertexDescription description{};

    static constexpr std::uint32_t binding{};
    static constexpr VkVertexInputBindingDescription mainBinding = { .binding = binding,
                                                                     .stride = sizeof(vertex_t),
                                                                     .inputRate = VK_VERTEX_INPUT_RATE_VERTEX };
    description.bindings.push_back(mainBinding);

    // Attributes
    auto location = static_cast<std::uint32_t>(description.attributes.size());
    description.attributes.push_back(position_attribute(binding, location));
    description.attributes.push_back(normal_attribute(binding, ++location));
    description.attributes.push_back(tangent_attribute(binding, ++location));
    description.attributes.push_back(texcoord_0_attribute(binding, ++location));
    description.attributes.push_back(texcoord_1_attribute(binding, ++location));

    return description;
}
}    // namespace odin::graphics::vk::resource
