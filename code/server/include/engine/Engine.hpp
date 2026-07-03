#pragma once

#include "core/vulkan/VulkanInitializer.hpp"
#include "engine/gfx/Renderer.hpp"

namespace engine {

class Engine {
private:
	gfx::Renderer renderer;
	core::vk::VulkanInitializer vulkan;

public:
	void Init();
	void Run();
	void Destroy();
};

}
