#pragma once

#include "SDL3/SDL_events.h"

namespace engine::core::sdl {
	class Window; 
}

namespace engine::core::sdl::event {
	class EventManager {
	private:
		SDL_Event event;
		bool quit = false;
		bool stopRendering = false;
	public:
		void PollEvents(engine::core::sdl::Window *window);
	};
};
