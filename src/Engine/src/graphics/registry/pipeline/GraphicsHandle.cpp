#include "GraphicsHandle.hpp"
//
//
namespace odin::graphics::registry::pipeline
{
GraphicsHandle::GraphicsHandle(std::shared_ptr<GraphicsSlot> pSlot)
    : Handle<GraphicsHandle, GraphicsSlot, GraphicsResource>(std::move(pSlot))
{}
}    // namespace odin::graphics::registry::pipeline
