//
// Created by qwerty on 22/07/2024.
//
#pragma once

#include <memory>
#include <filesystem>
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
private:
    std::unique_ptr<Impl> m_pImpl;
};
}    // namespace asl
