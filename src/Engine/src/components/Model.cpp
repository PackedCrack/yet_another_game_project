#include "Model.hpp"
//
//
namespace odin::component
{
Model::Model(std::filesystem::path path)
    : filepath{ std::move(path) }
    , handle{ nullptr }
    , submeshes{}
{}
}    // namespace odin::component
