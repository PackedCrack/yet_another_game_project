//
// Created by qwerty on 29/07/2025.
//
#pragma once

//
//
namespace odin::graphics::registry::resource::shader
{
class ShaderCompiler
{
	class Impl;
public:
	ShaderCompiler(std::filesystem::path shader, std::filesystem::path output);
	~ShaderCompiler();
	ShaderCompiler(const ShaderCompiler& other) = delete;
	ShaderCompiler(ShaderCompiler&& other) noexcept;
	ShaderCompiler& operator=(const ShaderCompiler& other) = delete;
	ShaderCompiler& operator=(ShaderCompiler&& other) noexcept;
public:
	void compile() const;
private:
	std::unique_ptr<Impl> m_pImpl;
};
}	// namespace odin::graphics::registry::resource::shader