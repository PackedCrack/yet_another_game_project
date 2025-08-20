//
// Created by qwerty on 20/08/2025.
//
#pragma once

#include "ArenaAllocator.hpp"
#include "graphics/vk/resource/VertexBuffer.hpp"
//
//
namespace odin
{
class MeshRegistry
{
	using Vertex = graphics::vk::resource::Vertex;
public:
private:
	ArenaAllocator<Vertex> m_VertexTracker;
};
}	// namespace odin