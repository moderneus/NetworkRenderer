#pragma once

#include <deque>
#include <functional>

namespace core::vk {

class DeletionQueue {
private:
  std::deque<std::function<void()>> deletionQueue;

public:
  void Push(std::function<void()>);
  void CleanUp();
};

} // namespace core::vk
