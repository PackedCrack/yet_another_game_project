#include "Surface.hpp"

#include "debug/Logger.hpp"
// sdl
#include <SDL3/SDL_vulkan.h>
//
//
namespace odin::graphics::vk
{
Surface::Surface(window::Window& wnd, const Instance& instance)
	: m_Surface{ wnd.make_surface(instance) }
	, m_Instance{ instance.handle() }
{}
Surface::~Surface()
{
	if (m_Surface != VK_NULL_HANDLE)
	{
		SDL_Vulkan_DestroySurface(m_Instance, m_Surface, nullptr);
	}
}
Surface::Surface(Surface&& other) noexcept
	: m_Surface{ std::exchange(other.m_Surface, VK_NULL_HANDLE) }
	, m_Instance{ std::exchange(other.m_Instance, VK_NULL_HANDLE) }
{}
Surface& Surface::operator=(Surface&& other) noexcept
{
	if (this != std::addressof(other))
	{
		m_Surface = std::exchange(other.m_Surface, VK_NULL_HANDLE);
		m_Instance = std::exchange(other.m_Instance, VK_NULL_HANDLE);
	}

	return *this;
}
VkSurfaceKHR Surface::handle() const
{
	ODIN_ASSERT(m_Surface != VK_NULL_HANDLE);
	return m_Surface;
}
}    // namespace odin::graphics::vk
