#include "engine/window/Window.hpp"

#include "SDL3/SDL.h"
#include "SDL3/SDL_vulkan.h"

#include "fmt/core.h"
#include "fmt/color.h"

void engine::core::sdl::Window::Init() {
	handle = SDL_CreateWindow("Renderer", extent.width, extent.height, SDL_WINDOW_VULKAN);
	if (!handle)
		fmt::print(fmt::fg(fmt::color::red), "[SDL] ERROR: {}\n", SDL_GetError());
	else {
		isInitialized = true;
		isClosed = false;
	}
}

void engine::core::sdl::Window::Destroy() {
	if (isInitialized)
		SDL_DestroyWindow(handle);
	else 
		fmt::print(fmt::fg(fmt::color::red), "[SDL] ERROR: Trying to destroy the window while it is uninitialized\n");
}

bool engine::core::sdl::Window::IsClosed() {
	return isClosed;
}

void engine::core::sdl::Window::Close() {
	isClosed = true;
}
