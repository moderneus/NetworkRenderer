#pragma once

#include "core/vulkan/DeletionQueue.hpp"
#include "core/vulkan/VulkanHelper.hpp"

#include <vulkan/vulkan.h>

namespace core::vk {

class VulkanInitializer {
private:
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
