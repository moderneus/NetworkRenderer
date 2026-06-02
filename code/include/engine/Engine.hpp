#pragma once

#include "core/SDL/SDLInitializer.hpp"
#include "engine/gfx/Renderer.hpp"
#include "engine/events/EventManager.hpp"
#include "window/Window.hpp"

namespace engine {
	class Engine {
	private:
		engine::core::sdl::SDLInitializer sdl;
		engine::core::sdl::event::EventManager eventManager;
		core::sdl::Window window;
		gfx::Renderer renderer;

		bool isInitialized = false;
	public:
		static Engine& Get();
		void Init();
		void Run();
		void Destroy();
	};
}
