#pragma once

#include "core/vulkan/DeletionQueue.hpp"
#include "core/vulkan/VulkanHelper.hpp"

#include <vulkan/vulkan.h>

namespace core::vk {

class VulkanInitializer {
private:
  std::vector<const char *> layerNames = { "VK_LAYER_KHRONOS_validation" };
  std::vector<const char *> extensionNames = { "VK_EXT_debug_utils" };

  DeletionQueue deletionQueue;
  VulkanHelper vulkanHelper;

  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo;
  VkPhysicalDevice physicalDevice;

  void CreateInstance();
  void CreateDebugMessenger();
  void PickPhysicalDevice();

public:
  void Init();
  void Destroy();
};

} // namespace core::vk
