//
// Created by qwerty on 11/08/2025.
//
#pragma once

#include "OdinInfo.hpp"
#include "ECS.hpp"
#include "Core.hpp"
//
//
namespace odin
{
class Odin
{
    class Impl;
public:
    Odin(OdinInfo info);
    ~Odin();
    Odin(Odin&& other) noexcept;
    Odin& operator=(Odin&& other) noexcept;
public:
    void begin_frame();
    void render();
    void end_frame();
    [[nodiscard]] bool running() const;
private:
    std::unique_ptr<Impl> m_pImpl;
};
}    // namespace odin
