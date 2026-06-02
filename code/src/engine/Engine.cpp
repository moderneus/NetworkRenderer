#include "engine/Engine.hpp"

#include <cassert>

engine::Engine *instance = nullptr;

engine::Engine& engine::Engine::Get() {
	return *instance;
}

void engine::Engine::Init() {
	assert(instance == nullptr);
	instance = this;

	sdl.Init();
	window.Init();
	
	isInitialized = true;
}

void engine::Engine::Destroy() {
	window.Destroy();
	sdl.Destroy();
	instance = nullptr;
}


void engine::Engine::Run() {
	while(!window.IsClosed()) {
		eventManager.PollEvents(&window);
		renderer.Draw();
	}
}
