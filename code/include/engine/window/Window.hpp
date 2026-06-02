#pragma once

#include <vulkan/vulkan.h>

struct SDL_Window;

namespace engine::core::sdl {
	class Window {
	private:
		SDL_Window* handle = nullptr;
		VkExtent2D extent = {640, 480};
		bool isInitialized = false;
		bool isClosed = true;
	public:
		void Init();
		void Destroy();
		bool IsClosed();
		void Close();
	};
}
