//
// Created by qwerty on 11/08/2025.
//
#pragma once

#include "../OdinInfo.hpp"
//
//
namespace odin::graphics
{
class Graphics
{
    class Impl;
public:
    Graphics(const OdinInfo& info);
    ~Graphics();
    Graphics(Graphics&& other) noexcept;
    Graphics& operator=(Graphics&& other) noexcept;
private:
    std::unique_ptr<Impl> m_pImpl;
};
}    // namespace odin::graphics
