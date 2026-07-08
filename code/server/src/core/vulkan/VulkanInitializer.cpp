#include "core/vulkan/VulkanInitializer.hpp"
#include "utils/Defines.hpp"

#include <cstdint>

void core::vk::VulkanInitializer::CreateInstance() {
  VkApplicationInfo appInfo{
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pApplicationName = "NetworkRenderer",
    .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
    .pEngineName = "No Engine",
    .engineVersion = VK_MAKE_VERSION(0, 1, 0),
    .apiVersion = VK_MAKE_API_VERSION(0, 1, 4, 0),
  };

  VkInstanceCreateInfo info{
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pNext = nullptr,
    .pApplicationInfo = &appInfo,
    .enabledLayerCount = 0,
    .ppEnabledLayerNames = nullptr,
    .enabledExtensionCount = 0,
    .ppEnabledExtensionNames = nullptr,
  };

  std::vector<const char *> layerNames = { "VK_LAYER_KHRONOS_validation" };

  if (vulkanHelper.CheckValidationLayersSupport(layerNames)) {
    info.enabledLayerCount = static_cast<std::uint32_t>(layerNames.size());
    info.ppEnabledLayerNames = layerNames.data();
  }

  std::vector<const char *> extensionNames = { "VK_EXT_debug_utils" };

  if (vulkanHelper.CheckExtensionsSupport(extensionNames)) {
    info.enabledExtensionCount =
        static_cast<std::uint32_t>(extensionNames.size());
    info.ppEnabledExtensionNames = extensionNames.data();
    debugMessengerInfo = {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = vulkanHelper.Callback,
      .pUserData = nullptr,
    };
    info.pNext = &debugMessengerInfo;
  }

  VK_CHECK(vkCreateInstance(&info, nullptr, &instance));

  deletionQueue.Push([this]() { vkDestroyInstance(instance, nullptr); });
}

void core::vk::VulkanInitializer::CreateDebugMessenger() {
  PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT;
  pfnVkCreateDebugUtilsMessengerEXT =
      (PFN_vkCreateDebugUtilsMessengerEXT)(vkGetInstanceProcAddr(
          instance, "vkCreateDebugUtilsMessengerEXT"));

  if (!pfnVkCreateDebugUtilsMessengerEXT)
    fmt::print(fmt::fg(fmt::color::dark_red),
        "[VULKAN] ERROR: Cannot create a DebugMessenger: "
        "pfnVkCreateDebugUtilsMessengerEXT is nullptr\n");

  VK_CHECK(pfnVkCreateDebugUtilsMessengerEXT(
      instance, &debugMessengerInfo, nullptr, &debugMessenger));

  deletionQueue.Push([this]() {
    PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;
    pfnVkDestroyDebugUtilsMessengerEXT =
        (PFN_vkDestroyDebugUtilsMessengerEXT)(vkGetInstanceProcAddr(
            instance, "vkDestroyDebugUtilsMessengerEXT"));

    if (!pfnVkDestroyDebugUtilsMessengerEXT)
      fmt::print(fmt::fg(fmt::color::dark_red),
          "[VULKAN] ERROR: Cannot destroy a DebugMessenger: "
          "pfnVkDestroyDebugUtilsMessengerEXT is nullptr\n");

    pfnVkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
  });
}

void core::vk::VulkanInitializer::Init() {
  CreateInstance();
  CreateDebugMessenger();
}

void core::vk::VulkanInitializer::Destroy() { deletionQueue.CleanUp(); }
