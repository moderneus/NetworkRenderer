#include "engine/Engine.hpp"

void engine::Engine::Init() {
  vulkan.Init();
}

void engine::Engine::Destroy() {
  vulkan.Destroy();
}


void engine::Engine::Run() {
  renderer.Draw();
}
