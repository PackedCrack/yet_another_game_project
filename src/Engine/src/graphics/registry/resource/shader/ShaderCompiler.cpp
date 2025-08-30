//
// Created by qwerty on 29/08/2025.
//
#include "ShaderCompiler.hpp"

// debug
#include <debug/debug_defines.hpp>
#include <debug/Logger.hpp>
// shaderc
#include <shaderc/shaderc.hpp>
//
//
namespace
{
[[nodiscard]] std::filesystem::path resolve_included_file_path(const char* requested_source, const char* requesting_source)
{
    std::filesystem::path fileToInclude{ requested_source };
    std::filesystem::path source{ requesting_source };
    std::filesystem::path rootDirectory = source.parent_path();

    ODIN_ASSERT(std::filesystem::is_directory(rootDirectory));
    for (auto&& entry : std::filesystem::directory_iterator{ rootDirectory })
    {
        if (entry.is_regular_file())
        {
            if (entry.path().filename() == fileToInclude)
            {
                return entry.path();
            }
        }
    }

    throw std::runtime_error(std::format("Unable to find file: {}", requested_source));
}
struct Owner
{
    std::string includedFile;
    std::vector<char> includedContent;
};
struct Includer : public shaderc::CompileOptions::IncluderInterface
{
    shaderc_include_result*
    GetInclude(const char* requested_source, shaderc_include_type type, const char* requesting_source, size_t include_depth) override
    {
        ODIN_ASSERT(type == shaderc_include_type_relative);

        try
        {
            std::filesystem::path includedFile = resolve_included_file_path(requested_source, requesting_source);
            std::fstream file(includedFile, std::ios::binary | std::ios::in);
            std::uintmax_t fileSize = std::filesystem::file_size(includedFile);

            auto pOwner = new Owner{};
            pOwner->includedFile = includedFile.string();
            pOwner->includedContent.resize(fileSize);
            file.read(pOwner->includedContent.data(), pOwner->includedContent.size());

            auto pResult = new shaderc_include_result{};
            pResult->source_name = pOwner->includedFile.data();
            pResult->source_name_length = pOwner->includedFile.size();
            pResult->content = pOwner->includedContent.data();
            pResult->content_length = pOwner->includedContent.size();
            pResult->user_data = pOwner;

            return pResult;
        }
        catch (const std::runtime_error& err)
        {
            LOG_ERR("{}", err.what());
            return nullptr;
        }
    }
    void ReleaseInclude(shaderc_include_result* data) override
    {
        // dumbasses don't use RAII so now we have to run DrMemory
        if (data != nullptr)
        {
            Owner* pOwner = static_cast<Owner*>(data->user_data);
            delete pOwner;
            delete data;
        }
    }
};
[[nodiscard]] std::string load_source_code(const std::filesystem::path& shader)
{
    ODIN_ASSERT(std::filesystem::exists(shader));
    ODIN_ASSERT(std::filesystem::is_regular_file(shader));

    std::vector<char> fileContent{};
    fileContent.resize(std::filesystem::file_size(shader));

    std::fstream file{ shader, std::ios::binary | std::ios::in };
    file.read(fileContent.data(), fileContent.size());

    // shaderc wants the code as string for some reason..
    std::string source{};
    source.resize(fileContent.size());
    std::memcpy(source.data(), fileContent.data(), source.size());

    return source;
}
[[nodiscard]] shaderc_shader_kind shader_type(const std::filesystem::path& shader)
{
    std::string extension = shader.extension().string();
    if (extension == ".vert")
    {
        return shaderc_glsl_default_vertex_shader;
    }
    else if (extension == ".frag")
    {
        return shaderc_glsl_default_fragment_shader;
    }
    else if (extension == ".comp")
    {
        return shaderc_glsl_default_compute_shader;
    }

    ODIN_ASSERT(false);    // "Unknown shader extension"
    return shaderc_glsl_infer_from_source;
}
}    // namespace
namespace odin::graphics::registry::resource::shader
{
class ShaderCompiler::Impl
{
public:
    Impl(std::filesystem::path shader, std::filesystem::path output)
        : m_Compiler{}
        , m_Options{}
        , m_Filename{ std::move(shader) }
        , m_Output{ std::move(output) }
        , m_SourceCode{ load_source_code(m_Filename) }
        , m_Type{ shader_type(m_Filename) }
    {
        set_compile_options();
    }
public:
    void compile() const
    {
        std::string filename = m_Filename.string();
        shaderc::SpvCompilationResult result = m_Compiler.CompileGlslToSpv(m_SourceCode, m_Type, filename.data(), m_Options);
        shaderc_compilation_status status = result.GetCompilationStatus();
        if (status == shaderc_compilation_status_success)
        {
            std::size_t numWarn = result.GetNumWarnings();
            if (numWarn > 0)
            {
                LOG_WARN("Warnings when compiling {}.\n{}", filename.c_str(), result.GetErrorMessage().c_str());
            }

            LOG_INFO("Successfully compiled {}.", filename.c_str());
            save_spirv_data(result);
        }
        else if (status == shaderc_compilation_status_compilation_error)
        {
            std::string msg = result.GetErrorMessage();
            LOG_ERR("Compilation of {} failed with {}.", filename.c_str(), msg.c_str());
        }
        else
        {
            LOG_ERR("Compilation of {} with status code: #{}", filename.c_str(), static_cast<std::int32_t>(status));
        }
    }
private:
    void set_compile_options()
    {
        m_Options.SetOptimizationLevel(shaderc_optimization_level_performance);
        m_Options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
        m_Options.SetSourceLanguage(shaderc_source_language_glsl);
        m_Options.SetIncluder(std::make_unique<Includer>());
    }
    void save_spirv_data(const shaderc::SpvCompilationResult& result) const
    {
        using DWORD = std::uint32_t;

        std::vector<DWORD> spirv{ std::begin(result), std::end(result) };
        std::fstream file{ m_Output, std::ios::binary | std::ios::out | std::ios::trunc };
        auto size = static_cast<std::streamsize>(spirv.size() * sizeof(DWORD));
        file.write(reinterpret_cast<const char*>(spirv.data()), size);
    }
private:
    shaderc::Compiler m_Compiler;
    shaderc::CompileOptions m_Options;
    std::filesystem::path m_Filename;
    std::filesystem::path m_Output;
    std::string m_SourceCode;
    shaderc_shader_kind m_Type;
};
//
//	Pimpl
//
ShaderCompiler::ShaderCompiler(std::filesystem::path shader, std::filesystem::path output)
    : m_pImpl{ std::make_unique<Impl>(std::move(shader), std::move(output)) }
{}
ShaderCompiler::~ShaderCompiler() = default;
ShaderCompiler::ShaderCompiler(ShaderCompiler&& other) noexcept = default;
ShaderCompiler& ShaderCompiler::operator=(ShaderCompiler&& other) noexcept = default;
void ShaderCompiler::compile() const
{
    m_pImpl->compile();
}
}    // namespace odin::graphics::registry::resource::shader
