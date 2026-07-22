#pragma once

#include "volk.h"

#include <cstdint>
#include <vector>

namespace core::vk {

class VulkanHelper {
  friend class VulkanInitializer;

  bool CheckValidationLayersSupport(
      const std::vector<const char *> &layerNames);

  bool CheckExtensionSupport(const char *extensionName,
      const std::vector<VkExtensionProperties> &availableExtensions);
  bool CheckInstanceExtensionsSupport(
      const std::vector<const char *> &extensionNames);
  bool CheckDeviceExtensionsSupport(const VkPhysicalDevice &physicalDevice,
      const std::vector<const char *> &extensionNames);

  bool CheckImageFormatSupport(
      const VkFormat &format, const VkPhysicalDevice &physicalDevice);

  std::uint32_t RatePhysicalDevice(const VkPhysicalDevice &physicalDevice);

  static VKAPI_ATTR VkBool32 VKAPI_CALL Callback(
      VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
      VkDebugUtilsMessageTypeFlagsEXT messageType,
      const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
      void *pUserData);
};

} // namespace core::vk
