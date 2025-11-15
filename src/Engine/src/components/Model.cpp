#include "Model.hpp"
//
//
namespace odin::component
{
Model::Model(asl::UUID uuid)
    : uuid{ uuid }
    , handle{ nullptr }
    , submeshes{}
{}
}    // namespace odin::component
