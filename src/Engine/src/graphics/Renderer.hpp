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
                      vk::QueueView graphics,
                      const FrameContext& frameContext,
                      std::optional<TransferEpoch>& transferEpoch);
private:
};
}    // namespace odin::graphics
