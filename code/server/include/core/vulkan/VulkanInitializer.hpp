#pragma once

#include "core/vulkan/DeletionQueue.hpp"

#include <vector>
#include <vulkan/vulkan.h>

namespace core::vk {

class VulkanInitializer {
private:
  DeletionQueue deletionQueue;

  VkInstance instance;
  // VkDebugUtilsMessengerEXT debugMessenger;

  bool CheckValidationLayerSupport(const std::vector<const char *> layerNames);

  void CreateInstance();
  void CreateDebugMessenger();

public:
  void Init();
  void Destroy();
};

} // namespace core::vk
