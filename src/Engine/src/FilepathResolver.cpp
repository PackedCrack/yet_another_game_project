#include "FilepathResolver.hpp"

// debug
#include <debug/debug_defines.hpp>
//
//
namespace
{
[[nodiscard]] std::filesystem::path make_root_dir(int argc, char** argv)
{
    ODIN_ASSERT(argc > 0);
    return std::filesystem::path{ argv[0] }.parent_path();
}
}    // namespace
namespace odin
{
FilepathResolver& FilepathResolver::get(int argc, char** argv)
{
    static FilepathResolver resolver{ argc, argv };
    return resolver;
}
FilepathResolver::FilepathResolver(int argc, char** argv)
    : m_Root{ make_root_dir(argc, argv) }
    , m_ShaderDir{ m_Root / "../../resources/shaders" }
{}
std::filesystem::path FilepathResolver::resolve_shader_path(std::string_view filename) const
{
    return m_ShaderDir / filename;
}
}    // namespace odin
