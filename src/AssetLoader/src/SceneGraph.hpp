//
// Created by qwerty on 22/07/2024.
//
#pragma once

#include "common_types.hpp"
// std
#include <memory>
#include <filesystem>
#include <functional>
//
//
namespace asl
{
class SceneGraph
{
    class Impl;
public:
    explicit SceneGraph(std::filesystem::path filename);
    ~SceneGraph();
    SceneGraph(const SceneGraph& other) = delete;
    SceneGraph(SceneGraph&& other) noexcept;
    SceneGraph& operator=(const SceneGraph& other) = delete;
    SceneGraph& operator=(SceneGraph&& other) noexcept;
public:
    // Pointers are only valid insider the visitor function - dont store
    void dfs(std::function<void(const NodeView*, const NodeView*)> visitor) const;
    [[nodiscard]] const std::filesystem::path& filename() const;    // should be uuid probably
private:
    std::unique_ptr<Impl> m_pImpl;
};
}    // namespace asl
