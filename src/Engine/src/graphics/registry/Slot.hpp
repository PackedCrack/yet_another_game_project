//
// Created by qwerty on 26/08/2025.
//
#pragma once
//
//
namespace odin::graphics::registry
{
template<typename derived_t, typename resource_t>
struct Slot : public std::enable_shared_from_this<derived_t>
{
    std::mutex initMutex;
    std::atomic<std::shared_ptr<const resource_t>> pResource;
};
}    // namespace odin::graphics::registry
