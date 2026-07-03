#include "core/vulkan/DeletionQueue.hpp"

void core::vk::DeletionQueue::Push(std::function<void()> destroyFunction) {
  deletionQueue.push_back(destroyFunction);
}

void core::vk::DeletionQueue::CleanUp() {
  for(auto it = deletionQueue.rbegin(); it != deletionQueue.rend(); ++it) {
    (*it)();
  }
}
