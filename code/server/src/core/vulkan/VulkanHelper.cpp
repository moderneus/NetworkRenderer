#include "core/vulkan/VulkanHelper.hpp"

#include "fmt/color.h"
#include "fmt/core.h"

#include <cstdint>
#include <cstring>

bool core::vk::VulkanHelper::CheckValidationLayersSupport(
    const std::vector<const char *> &layerNames) {
  std::uint32_t layersCount = 0;
  vkEnumerateInstanceLayerProperties(&layersCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layersCount);
  vkEnumerateInstanceLayerProperties(&layersCount, availableLayers.data());

  for (const char *layerName : layerNames) {
    bool isFound = false;

    for (const auto &layer : availableLayers) {
      if (strcmp(layerName, layer.layerName) == 0) {
        isFound = true;
        break;
      }
    }

    if (!isFound)
      return false;
  }

  return true;
}

bool core::vk::VulkanHelper::CheckExtensionSupport(const char *extensionName,
    const std::vector<VkExtensionProperties> &availableExtensions) {
  bool isFound = false;

  for (const auto &extension : availableExtensions) {
    if (strcmp(extensionName, extension.extensionName) == 0) {
      isFound = true;
      break;
    }
  }

  return isFound;
}

bool core::vk::VulkanHelper::CheckInstanceExtensionsSupport(
    const std::vector<const char *> &extensionNames) {
  std::uint32_t extensionsCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionsCount);
  vkEnumerateInstanceExtensionProperties(
      nullptr, &extensionsCount, availableExtensions.data());

  for (const char *extensionName : extensionNames) {
    bool isExtensionSupported =
        CheckExtensionSupport(extensionName, availableExtensions);

    if (!isExtensionSupported) {
      fmt::print(fmt::fg(fmt::color::dark_red),
          "[VULKAN] ERROR: Extension {} is not supported\n", extensionName);
      return false;
    }
  }

  return true;
}

bool core::vk::VulkanHelper::CheckDeviceExtensionsSupport(
    const VkPhysicalDevice &physicalDevice,
    const std::vector<const char *> &extensionNames) {
  std::uint32_t extensionCount = 0;
  vkEnumerateDeviceExtensionProperties(
      physicalDevice, nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(
      physicalDevice, nullptr, &extensionCount, availableExtensions.data());

  for (const char *extensionName : extensionNames) {
    bool isExtensionSupported =
        CheckExtensionSupport(extensionName, availableExtensions);

    if (!isExtensionSupported) {
      fmt::print(fmt::fg(fmt::color::dark_red),
          "[VULKAN] ERROR: Extension {} is not supported\n", extensionName);
      return false;
    }
  }

  return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL core::vk::VulkanHelper::Callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData) {
  fmt::color severityColor;

  switch (messageSeverity) {
  default:
    severityColor = fmt::color::white;
    break;

  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    severityColor = fmt::color::gray;
    break;

  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    severityColor = fmt::color::gold;
    break;

  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    severityColor = fmt::color::red;
    break;

  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    severityColor = fmt::color::blue;
    break;
  }

  fmt::print(fmt::fg(fmt::color::dark_red), "[VULKAN] ");
  fmt::print(fmt::fg(severityColor), "{}: ", pCallbackData->pMessageIdName);
  fmt::print(fmt::fg(fmt::color::white), "{}\n", pCallbackData->pMessage);

  return VK_FALSE;
}

std::uint32_t core::vk::VulkanHelper::RatePhysicalDevice(
    const VkPhysicalDevice &physicalDevice) {
  std::uint32_t score = 0;

  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(physicalDevice, &properties);

  if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    score += 1000;

  score += properties.limits.maxSamplerAnisotropy;
  score += properties.limits.maxImageDimension2D;

  return score;
}
