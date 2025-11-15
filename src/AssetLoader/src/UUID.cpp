#include "UUID.hpp"
#include <cassert>    // TODO DO NOT USE. use odin assert instead
namespace
{
std::array<std::uint8_t, 16> hex_to_byte_array(const std::string& hex)
{
    if (hex.size() != 32)
    {
        assert(false);
    }

    std::array<std::uint8_t, 16> bytes{};
    for (size_t i = 0; i < 16; ++i)
    {
        std::string byteStr = hex.substr(i * 2, 2);    // 0*2=0, 1*2=2, 2*2=4... etc
        bytes[i] = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
    }
    return bytes;
}
}    // Namespace
namespace asl
{
UUID::UUID(const std::string& uuid)
    : m_Bytes(hex_to_byte_array(uuid))
{}
}    // namespace asl
