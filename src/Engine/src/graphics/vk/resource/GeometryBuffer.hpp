//
// Created by qwerty on 19/08/2025.
//
#pragma once

#include "debug/Logger.hpp"
//
//
namespace odin::graphics::vk::resource
{
class GeometryBuffer
{
public:
    GeometryBuffer(std::uint32_t size);
    ~GeometryBuffer() = default;
    GeometryBuffer(const GeometryBuffer& other) = delete;
    GeometryBuffer(GeometryBuffer&& other) noexcept;
    GeometryBuffer& operator=(const GeometryBuffer& other) = delete;
    GeometryBuffer& operator=(GeometryBuffer&& other) noexcept;
    template<typename buffer_t>
    requires std::ranges::contiguous_range<typename std::remove_cvref_t<buffer_t>>
    [[nodiscard]] std::uint32_t queue_transfer(buffer_t&& data /*, CUploadContext& uploadContext*/)
    {
        ODIN_ASSERT(!data.empty());

        std::size_t bytesToWrite = data.size() * sizeof(typename std::remove_cvref_t<buffer_t>::element_type);
        std::uint32_t bytesRemain = m_BufferSize - m_TransferOffset;
        if (bytesToWrite > bytesRemain)
        {
            LOG_FATAL("Geometry Buffer ran out of pre allocated memory!");
        }

        //m_Buffer.map_memory(uploadContext, data, bytesToWrite, m_WriteOffset);
        m_TransferOffset += static_cast<std::uint32_t>(data.size());

        return m_TransferOffset;
    };
private:
    std::uint32_t m_BufferSize;
    std::uint32_t m_TransferOffset;
};
}    // namespace odin::graphics::vk::resource
