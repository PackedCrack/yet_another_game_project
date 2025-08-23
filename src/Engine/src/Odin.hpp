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
    enum class State
    {
        begin,
        graphics,
        end
    };
public:
    Odin(OdinInfo info);
public:
    void begin_frame();
    void render();
    void end_frame();
    [[nodiscard]] std::unique_ptr<ECS> make_ecs();
private:
private:
    State m_State;
    // AssetRegistry
    graphics::Graphics m_Gfx;
    std::optional<std::reference_wrapper<ECS>> m_ECS;
};
}    // namespace odin
