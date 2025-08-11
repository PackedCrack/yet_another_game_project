#pragma once
//
//
namespace odin::graphics
{
class Graphics
{
    class Impl;
public:
    Graphics(const OdinInfo& info);
private:
    std::unique_ptr<Impl> m_pImpl;
};
}    // namespace odin::graphics
