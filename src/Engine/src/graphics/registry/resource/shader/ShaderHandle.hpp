//
// Created by qwerty on 26/08/2025.
//
#pragma once

#include "ShaderSlot.hpp"
#include "../../Handle.hpp"
#include "../../../vk/resource/ShaderModule.hpp"
//
//
namespace odin::graphics::registry::resource::shader
{
struct ShaderHandle
{
public:
    ShaderHandle(std::shared_ptr<ShaderSlot> pSlot, std::function<void()> hot_reload);
    [[nodiscard]] std::shared_ptr<const vk::resource::ShaderModule> acquire() const;
    [[nodiscard]] std::uintptr_t id() const;
    inline operator bool() const { return m_pSlot != nullptr; }
private:
    std::shared_ptr<ShaderSlot> m_pSlot;
    std::function<void()> PFN_hot_reload;
};
}    // namespace odin::graphics::registry::resource::shader
