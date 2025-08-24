//
// Created by qwerty on 24/07/2025.
//
#pragma once

#include "SceneGraph.hpp"

// std
#include <atomic>
//
//
namespace asl
{
struct ModelSlot : public std::enable_shared_from_this<ModelSlot>
{
    std::mutex initMutex;
    std::atomic<std::shared_ptr<const SceneGraph>> pGraph;
};
class ModelHandle
{
public:
    ModelHandle(std::shared_ptr<ModelSlot> pSlot);
    std::shared_ptr<const SceneGraph> acquire() const;
    operator bool() const { return m_pSlot != nullptr; }
private:
    std::shared_ptr<ModelSlot> m_pSlot;
};
}    // namespace asl
