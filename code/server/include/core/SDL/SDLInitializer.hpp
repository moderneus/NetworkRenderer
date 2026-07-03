#pragma once

namespace engine::core::sdl {
	class SDLInitializer {
	private:
		bool isInitialized = false;
	public:
		void Init();
		void Destroy();
	};
}
