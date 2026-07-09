#pragma once

#include "core/vulkan/DeletionQueue.hpp"
#include "core/vulkan/VulkanHelper.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>

namespace core::vk {

class VulkanInitializer {
private:
  std::vector<const char *> layerNames = { "VK_LAYER_KHRONOS_validation" };
  std::vector<const char *> extensionNames = { "VK_EXT_debug_utils" };

  DeletionQueue deletionQueue;
  VulkanHelper vulkanHelper;

  VkInstance instance = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
  VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  std::uint32_t gfxQueueFamilyIndex = -1;
  VkQueue gfxQueue;
  VkDevice device;

  void CreateInstance();
  void CreateDebugMessenger();
  void PickPhysicalDevice();
  void FindQueueFamily();
  void CreateDevice();

public:
  void Init();
  void Destroy();
};

} // namespace core::vk
