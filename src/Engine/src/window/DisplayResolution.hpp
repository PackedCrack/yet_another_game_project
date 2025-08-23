//
// Created by qwerty on 11/08/2025.
//
#pragma once
//
//
// nHD		16:9	640		360
// WXGA		16:9	1280	720
// HD		≈16:9	1360	768
// HD+		16:9	1600	900
// FHD		16:9	1920	1080
// QWXGA	16:9	2048	1152
// QHD		16:9	2560	1440
// UHD		16:9	3840	2160

namespace odin::window
{
namespace details
{
    template<typename derived_t>
    class Resolution
    {
    public:
        Resolution(std::uint32_t width, std::uint32_t height)
            : m_Width{ width }
            , m_Height{ height }
        {}
        [[nodiscard]] std::uint32_t width() const { return m_Width; }
        [[nodiscard]] std::uint32_t height() const { return m_Height; }
    protected:
        std::uint32_t m_Width;
        std::uint32_t m_Height;
    };
}    // namespace details
//////////////////////////////////
// HD+		16:9	1600	900 //
//////////////////////////////////
class HDPlus : public details::Resolution<HDPlus>
{
public:
    HDPlus();
};
///////////////////////////////////
// FHD		16:9	1920    1080 //
///////////////////////////////////
class FHD : public details::Resolution<FHD>
{
public:
    FHD();
};
// Use as type erasure wrapper
using DisplayResolution = std::variant<HDPlus, FHD>;
[[nodiscard]] constexpr std::uint32_t get_width(DisplayResolution resolution)
{
    std::uint32_t width{};
    std::visit([&width](auto&& res) { width = res.width(); }, resolution);

    return width;
}
[[nodiscard]] constexpr std::uint32_t get_height(DisplayResolution resolution)
{
    std::uint32_t height{};
    std::visit([&height](auto&& res) { height = res.height(); }, resolution);

    return height;
}
}    // namespace odin::window
