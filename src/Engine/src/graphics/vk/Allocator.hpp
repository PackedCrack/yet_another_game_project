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
#include "resource/IndexBuffer.hpp"
#include "resource/VertexBuffer.hpp"
// common
#include <common.hpp>
//
//
namespace odin::graphics::vk
{
class Allocator : public std::enable_shared_from_this<Allocator>
{
    using StagingBuffer = resource::StagingBuffer;
    class Impl;
public:
    Allocator(const Instance& instance, const PhysicalDevice& gpu, const Device& device);
    ~Allocator();
    Allocator(const Allocator& other) = delete;
    Allocator(Allocator&& other) noexcept;
    Allocator& operator=(const Allocator& other) = delete;
    Allocator& operator=(Allocator&& other) noexcept;
public:
    // TODO: all of these functions should be called make_*
    [[nodiscard]] StagingBuffer create_staging_buffer(std::uint64_t numElements, std::uint64_t elementSize);
    template<typename buffer_t>
    requires std::ranges::contiguous_range<buffer_t>
    [[nodiscard]] std::unique_ptr<StagingBuffer> to_staging_buffer(buffer_t&& data)
    {
        using element_t = typename std::remove_cvref_t<buffer_t>::value_type;

        auto pStaging = std::make_unique<StagingBuffer>(create_staging_buffer(data.size(), sizeof(element_t)));
        pStaging->write(std::forward<buffer_t>(data));

        return pStaging;
    }
    [[nodiscard]] resource::UniformBuffer create_uniform_buffer(const VkBufferCreateInfo& info);
    [[nodiscard]] resource::StorageBuffer create_storage_buffer(VkDeviceSize size);
    [[nodiscard]] resource::Image create_image_attachment(const VkImageCreateInfo& info);
    [[nodiscard]] resource::Image create_image_texture(const VkImageCreateInfo& info);
    [[nodiscard]] resource::IndexBuffer create_index_buffer(std::uint64_t numElements);
    [[nodiscard]] resource::VertexBuffer create_vertex_buffer(std::uint64_t numElements);
    void destroy_buffer(VkBuffer buffer, void* pAllocation, const void* pData) const;
    void destroy_image(VkImage image, void* pAllocation) const;
private:
    std::unique_ptr<Impl> m_pImpl;
};
}    // namespace odin::graphics::vk
