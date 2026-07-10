#include "core/vulkan/VulkanInitializer.hpp"
#include "utils/Defines.hpp"

#include <map>

void core::vk::VulkanInitializer::InitVolk() {
  if (volkInitialize() != VK_SUCCESS) {
    fmt::print(fmt::fg(fmt::color::dark_red),
        "[VULKAN] Failed to initialize Vulkan: there is no one installed\n");
    std::abort();
  }
}

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

  if (vulkanHelper.CheckValidationLayersSupport(layerNames)) {
    info.enabledLayerCount = static_cast<std::uint32_t>(layerNames.size());
    info.ppEnabledLayerNames = layerNames.data();
  }

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
  VK_CHECK(vkCreateDebugUtilsMessengerEXT(
      instance, &debugMessengerInfo, nullptr, &debugMessenger));

  deletionQueue.Push([this]() {
    vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
  });
}

void core::vk::VulkanInitializer::PickPhysicalDevice() {
  std::uint32_t physicalDeviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

  std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
  vkEnumeratePhysicalDevices(
      instance, &physicalDeviceCount, physicalDevices.data());

  if (!physicalDeviceCount)
    fmt::print(fmt::fg(fmt::color::dark_red),
        "[VULKAN] ERROR: Failed to pick a physical device: there is no one "
        "with Vulkan support\n");

  if (physicalDeviceCount == 1) {
    physicalDevice = physicalDevices[0];
    return;
  }

  std::multimap<std::uint32_t, VkPhysicalDevice> candidates;

  for (const auto &physicalDevice : physicalDevices) {
    std::uint32_t score = 0;

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
      score += 1000;

    score += properties.limits.maxSamplerAnisotropy;
    score += properties.limits.maxImageDimension2D;

    candidates.insert(std::make_pair(score, physicalDevice));
  }

  physicalDevice = candidates.rbegin()->second;
}

void core::vk::VulkanInitializer::FindQueueFamily() {
  std::uint32_t queueFamilyCount;
  vkGetPhysicalDeviceQueueFamilyProperties(
      physicalDevice, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(
      physicalDevice, &queueFamilyCount, queueFamilies.data());

  for (std::uint32_t i = 0; i < queueFamilyCount; ++i) {
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
      gfxQueueFamilyIndex = i;
  }

  if (gfxQueueFamilyIndex == -1)
    fmt::print(fmt::fg(fmt::color::dark_red),
        "[VULKAN] ERROR: Failed to find a suitable queue famiy\n");
}

void core::vk::VulkanInitializer::CreateDevice() {
  VkDeviceQueueCreateInfo queueInfo{
    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
    .queueFamilyIndex = gfxQueueFamilyIndex,
    .queueCount = 1,
    .pQueuePriorities = queuePriorities.data(),
  };

  VkDeviceCreateInfo info{
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .queueCreateInfoCount = 1,
    .pQueueCreateInfos = &queueInfo,
  };

  VK_CHECK(vkCreateDevice(physicalDevice, &info, nullptr, &device));

  vkGetDeviceQueue(device, gfxQueueFamilyIndex, 0, &gfxQueue);

  deletionQueue.Push([this]() { vkDestroyDevice(device, nullptr); });
}

void core::vk::VulkanInitializer::CreateAllocator() {
  VmaVulkanFunctions vulkanFunctions{
    .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
    .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
  };

  VmaAllocatorCreateInfo info{
    .physicalDevice = physicalDevice,
    .device = device,
    .pVulkanFunctions = &vulkanFunctions,
    .instance = instance,
    .vulkanApiVersion = VK_MAKE_API_VERSION(0, 1, 4, 0),
  };

  VK_CHECK(vmaCreateAllocator(&info, &allocator));

  deletionQueue.Push([this]() { vmaDestroyAllocator(allocator); });
}

void core::vk::VulkanInitializer::Init() {
  InitVolk();
  CreateInstance();
  LoadInstanceFunctions();
  CreateDebugMessenger();
  PickPhysicalDevice();
  FindQueueFamily();
  CreateDevice();
  LoadDeviceFunctions();
  CreateAllocator();
}

void core::vk::VulkanInitializer::Destroy() { deletionQueue.CleanUp(); }
