#pragma once
#include <array>
#include <string>
//
//

namespace asl
{
class UUID
{
public:
    constexpr UUID(const std::array<std::uint8_t, 16>& bytes)
        : m_Bytes{ bytes }
    {}
    UUID(const std::string& uuid);
    ~UUID() = default;
    UUID(const UUID& other) = default;
    UUID& operator=(const UUID& other) = default;
    UUID(UUID&& other) noexcept = delete;
    UUID& operator=(UUID&& other) noexcept = delete;
public:
    constexpr bool operator==(const UUID& other) const noexcept { return m_Bytes == other.m_Bytes; }
    constexpr bool operator!=(const UUID& other) const noexcept { return m_Bytes != other.m_Bytes; }
public:
    std::array<std::uint8_t, 16> m_Bytes;
};
struct UUIDHasher
{
    static constexpr std::size_t FAT_PRIME = 0x69'69'69'69'69'69'69'61;
    std::size_t operator()(const UUID& uuid) const noexcept
    {
        std::hash<uint16_t> hasher{};
        std::size_t result = 0;
        for (std::size_t i = 0; i < uuid.m_Bytes.size(); i += 2)
        {
            uint16_t byteValue = (uint16_t(uuid.m_Bytes[i + 1]) << 8) | uint16_t(uuid.m_Bytes[i]);
            std::size_t value = hasher(byteValue) * FAT_PRIME;
            result ^= value;
            result = (result << 8) | (result >> (sizeof(std::size_t) * 8 - 8));
            result ^= FAT_PRIME;
        }
        return result;
    }
};    // UUID class
}    // namespace asl
