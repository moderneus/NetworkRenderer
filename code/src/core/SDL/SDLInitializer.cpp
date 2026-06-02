#include "core/SDL/SDLInitializer.hpp"

#include "SDL3/SDL.h"

#include "fmt/core.h"
#include "fmt/color.h"

void engine::core::sdl::SDLInitializer::Init() {
	isInitialized = SDL_Init(SDL_INIT_VIDEO);
	if (!isInitialized)
		fmt::print(fmt::fg(fmt::color::red), "[SDL] ERROR: {}\n", SDL_GetError());
}

void engine::core::sdl::SDLInitializer::Destroy() {
	if (isInitialized)
		SDL_Quit();
	else
		fmt::print(fmt::fg(fmt::color::red), "[SDL] ERROR: Trying to quit SDL while it is uninitialized\n");
}
