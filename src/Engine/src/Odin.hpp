//
// Created by qwerty on 11/08/2025.
//
#pragma once

#include "OdinInfo.hpp"
#include "ECS.hpp"
#include "graphics/Graphics.hpp"
//
//
namespace odin
{
// May require a pimpl in the future
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
    [[nodiscard]] std::unique_ptr<ECS> make_ecs();
private:
    std::unique_ptr<Impl> m_pImpl;
};
}    // namespace odin
