//
// Created by qwerty on 29/07/2025.
//
#pragma once
//
//
namespace odin
{
class FilepathResolver
{
public:
    [[nodiscard]] static FilepathResolver& get(int argc = 0, char** argv = nullptr);
private:
    FilepathResolver(int argc, char** argv);
public:
    [[nodiscard]] std::filesystem::path resolve_shader_path(std::string_view filename) const;
private:
    std::filesystem::path m_Root;
    std::filesystem::path m_ShaderDir;
};
}    // namespace odin
