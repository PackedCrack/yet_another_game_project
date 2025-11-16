#include "Model.hpp"
//
//
namespace odin::component
{
Model::Model(std::filesystem::path path)
    : filepath{ std::move(path) }
    , handle{ nullptr }
    , root{}
    , nodes{}
{}
}    // namespace odin::component
