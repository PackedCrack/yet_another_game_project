#pragma once

#include "ColorAttachment.hpp"
#include "FrameHandler.hpp"
#include "TransferManager.hpp"
//
//
namespace odin::graphics
{
class Renderer
{
public:
public:
    void render_frame(const ColorAttachment& colorAttachment,
                      vk::QueueView graphicsQ,
                      const FrameContext& frameContext,
                      const TransferManager& transferManager);
private:
};
}    // namespace odin::graphics
