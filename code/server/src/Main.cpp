#include "engine/Engine.hpp"

int main() {
  engine::Engine engine;
  engine.Init();
  engine.Run();
  engine.Destroy();
}
