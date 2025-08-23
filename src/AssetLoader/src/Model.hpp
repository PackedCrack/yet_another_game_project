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
class Model
{
    class Impl;
public:
    explicit Model(std::filesystem::path filename);
    ~Model();
    Model(const Model& other) = delete;
    Model(Model&& other) noexcept;
    Model& operator=(const Model& other) = delete;
    Model& operator=(Model&& other) noexcept;
public:
    //[[nodiscard]] std::vector<RenderableView> view_renderables() const;
    // Pointers are only valid insider the visitor function - dont store
    void dfs(std::function<void(const NodeView*, const NodeView*)> visitor) const;
    [[nodiscard]] std::string filename() const;    // should be uuid probably
private:
    std::unique_ptr<Impl> m_pImpl;
};
}    // namespace asl
