#pragma once

#include "ColorAttachment.hpp"
#include "FrameHandler.hpp"
//
//
namespace odin::graphics
{
class Renderer
{
public:
public:
    void render_frame(const ColorAttachment& colorAttachment, vk::QueueView graphics, const FrameContext& frameContext);
private:
};
}    // namespace odin::graphics
