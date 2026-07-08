#include "core/vulkan/VulkanHelper.hpp"

#include "fmt/color.h"
#include "fmt/core.h"

#include <cstdint>
#include <cstring>

bool core::vk::VulkanHelper::CheckValidationLayersSupport(
    const std::vector<const char *> layerNames) {
  std::uint32_t layersCount = 0;
  vkEnumerateInstanceLayerProperties(&layersCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layersCount);
  vkEnumerateInstanceLayerProperties(&layersCount, availableLayers.data());

  bool isFound = false;

  for (const char *layerName : layerNames) {
    for (auto layer : availableLayers) {
      if (strcmp(layerName, layer.layerName) == 0)
        isFound = true;
    }
  }

  return isFound;
}

bool core::vk::VulkanHelper::CheckExtensionsSupport(
    const std::vector<const char *> extensionNames) {
  std::uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(
      nullptr, &extensionCount, availableExtensions.data());

  bool isFound = false;

  for (const char *extensionName : extensionNames) {
    for (auto extension : availableExtensions) {
      if (strcmp(extensionName, extension.extensionName) == 0)
        isFound = true;
    }
  }

  return isFound;
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
};
