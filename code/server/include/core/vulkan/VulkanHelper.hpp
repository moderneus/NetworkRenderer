#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace core::vk {

class VulkanHelper {
public:
  bool CheckValidationLayersSupport(const std::vector<const char *> layerNames);
  bool CheckExtensionsSupport(const std::vector<const char *> extensionNames);

  static VKAPI_ATTR VkBool32 VKAPI_CALL Callback(
      VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
      VkDebugUtilsMessageTypeFlagsEXT messageType,
      const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
      void *pUserData);
};

} // namespace core::vk
