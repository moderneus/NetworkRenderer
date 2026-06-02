#include "engine/events/EventManager.hpp"
#include "engine/window/Window.hpp"

#include <thread>
#include <chrono>

void engine::core::sdl::event::EventManager::PollEvents(engine::core::sdl::Window *window) {
	while(!quit) {
		while(!SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				quit = true;
				window->Close();
			}
			if (event.type == SDL_EVENT_WINDOW_MINIMIZED)
				stopRendering = true;
			if (event.type == SDL_EVENT_WINDOW_RESTORED)
				stopRendering = false;
		}

		if (stopRendering) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}
	}
}
