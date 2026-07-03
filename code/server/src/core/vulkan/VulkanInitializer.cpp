#include "core/vulkan/VulkanInitializer.hpp"
#include "utils/Defines.hpp"

bool core::vk::VulkanInitializer::CheckValidationLayerSupport(
    const std::vector<const char *> layerNames) {
  uint32_t layersCount = 0;
  vkEnumerateInstanceLayerProperties(&layersCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layersCount);
  vkEnumerateInstanceLayerProperties(&layersCount, availableLayers.data());

  bool isFound = false;

  for (const char *layerName : layerNames) {
    for (auto layer : availableLayers) {
      if (layerName == layer.layerName)
        isFound = true;
    }
  }

  return isFound;
}

void core::vk::VulkanInitializer::CreateInstance() {
  VkApplicationInfo appInfo{
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pApplicationName = "NetworkRenderer",
    .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
    .engineVersion = VK_MAKE_VERSION(0, 1, 0),
    .apiVersion = VK_MAKE_API_VERSION(0, 1, 4, 0),
  };

  VkInstanceCreateInfo info{
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pApplicationInfo = &appInfo,
    .enabledLayerCount = 0,
    .ppEnabledLayerNames = nullptr,
    .enabledExtensionCount = 0,
    .ppEnabledExtensionNames = nullptr,
  };

  std::vector<const char *> layerNames = { "VK_LAYER_KHRONOS_validation" };

  if (CheckValidationLayerSupport(layerNames)) {
    info.enabledLayerCount = static_cast<uint32_t>(layerNames.size());
    info.ppEnabledLayerNames = layerNames.data();
  }

  VK_CHECK(vkCreateInstance(&info, nullptr, &instance));

  deletionQueue.Push([this]() { vkDestroyInstance(instance, nullptr); });
}

void core::vk::VulkanInitializer::CreateDebugMessenger() {}

void core::vk::VulkanInitializer::Init() {
  CreateInstance();
  CreateDebugMessenger();
}

void core::vk::VulkanInitializer::Destroy() { deletionQueue.CleanUp(); }
