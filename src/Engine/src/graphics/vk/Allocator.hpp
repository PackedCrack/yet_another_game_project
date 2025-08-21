//
// Created by qwerty on 12/08/2025.
//
#pragma once

#include "Device.hpp"
#include "Instance.hpp"
#include "PhysicalDevice.hpp"
#include "resource/Image.hpp"
#include "resource/StagingBuffer.hpp"
#include "resource/StorageBuffer.hpp"
#include "resource/UniformBuffer.hpp"
#include "resource/VertexBuffer.hpp"
//
//
namespace odin::graphics::vk
{
class Allocator : public std::enable_shared_from_this<Allocator>
{
    class Impl;
public:
    Allocator(const Instance& instance, const PhysicalDevice& gpu, const Device& device);
    ~Allocator();
    Allocator(const Allocator& other) = delete;
    Allocator(Allocator&& other) noexcept;
    Allocator& operator=(const Allocator& other) = delete;
    Allocator& operator=(Allocator&& other) noexcept;
public:
    [[nodiscard]] resource::StagingBuffer create_staging_buffer(std::uint64_t numElements, std::uint64_t elementSize);
    [[nodiscard]] resource::UniformBuffer create_uniform_buffer(const VkBufferCreateInfo& info);
    [[nodiscard]] resource::StorageBuffer create_storage_buffer(const VkBufferCreateInfo& info);
    [[nodiscard]] resource::Image create_image_attachment(const VkImageCreateInfo& info);
    [[nodiscard]] resource::Image create_image_texture(const VkImageCreateInfo& info);
    [[nodiscard]] resource::VertexBuffer create_vertex_buffer(std::uint64_t numElements);
    void destroy_buffer(VkBuffer buffer, void* pAllocation, const void* pData) const;
    void destroy_image(VkImage image, void* pAllocation) const;
private:
    std::unique_ptr<Impl> m_pImpl;
};
}    // namespace odin::graphics::vk
