#include "FilepathResolver.hpp"

// debug
#include <debug/debug_defines.hpp>
//
//
namespace
{
[[nodiscard]] std::filesystem::path normalize(const std::filesystem::path& p)
{
    return std::filesystem::weakly_canonical(p);
}
[[nodiscard]] std::filesystem::path make_cwd(int argc, char** argv)
{
    auto cwd = std::filesystem::path{ argv[0] }.parent_path();
    std::filesystem::current_path(cwd);
    return normalize(cwd);
}
[[nodiscard]] std::filesystem::path
resolve_subdirectory_upwards(std::filesystem::path cwd, std::filesystem::path unique, std::size_t searchLimit = 5)
{
    std::vector<std::filesystem::path> matches{};

    for (std::size_t i = 0; i <= searchLimit; ++i)
    {
        if (cwd.empty())
        {
            break;
        }

        std::filesystem::path candidate = cwd / unique;
        if (std::filesystem::exists(candidate) && std::filesystem::is_directory(candidate))
        {
            matches.emplace_back(normalize(candidate));
        }
        cwd = cwd.parent_path();
    }

    if (matches.size() == 1)
    {
        return matches.front();
    }

    std::string paths{};
    for (auto&& p : matches)
    {
        paths += p.string();
        paths += "\n";
    }
    LOG_FATAL("Can't resolve {}. Found multiple matches: {}", unique.string().c_str(), paths.c_str());
}
[[nodiscard]] std::filesystem::path make_shader_directory(std::filesystem::path cwd)
{
    return resolve_subdirectory_upwards(std::move(cwd), "resources/shaders");
}
[[nodiscard]] std::filesystem::path get_resource_directory_location(std::filesystem::path cwd)
{
    return resolve_subdirectory_upwards(std::move(cwd), "resources");
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
    : m_Cwd{ make_cwd(argc, argv) }
    , m_ResourceDir{ get_resource_directory_location(m_Cwd) }
{}
std::filesystem::path FilepathResolver::resolve_shader_path(std::string_view filename) const
{
    return m_ResourceDir / "shaders" / filename;
}
std::filesystem::path FilepathResolver::get_gpu_types_header() const
{
    // Hard settings this.. not ideal but it will work for now.
    return normalize(m_ResourceDir / "../src/Engine/src/graphics/gpu_types.hpp");
}
std::filesystem::path FilepathResolver::get_asset_db() const
{
    return normalize(m_ResourceDir / "assets" / "asset-db.txt");
}
}    // namespace odin
