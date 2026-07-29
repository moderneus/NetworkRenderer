#pragma once

#include "core/vulkan/DeletionQueue.hpp"
#include "core/vulkan/VulkanHelper.hpp"
#include "utils/FileHelper.hpp"

#include "volk.h"

#include "vk_mem_alloc.h"

#include <array>
#include <cstdint>

namespace core::vk {

class VulkanInitializer {
private:
  friend class VulkanHelper;

  DeletionQueue deletionQueue;
  VulkanHelper vulkanHelper;
  utils::file::FileHelper fileHelper;

  const std::vector<const char *> layerNames = {
    "VK_LAYER_KHRONOS_validation"
  };
  const std::vector<const char *> instanceExtensionNames = {
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
  };
  const std::vector<const char *> deviceExtensionNames = {
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
  };
  const std::vector<char> vertexShaderCode =
      fileHelper.ReadFile("shaders/VertexShader.vert.spv");
  const std::vector<char> fragmentShaderCode =
      fileHelper.ReadFile("shaders/FragmentShader.frag.spv");

  VkInstance instance = VK_NULL_HANDLE;

  VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
  VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo = {
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

  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeature;

  VkDevice device = VK_NULL_HANDLE;
  std::uint32_t gfxQueueFamilyIndex = -1;
  std::array<const float, 1> queuePriorities = { 1.0 };

  VkQueue gfxQueue = VK_NULL_HANDLE;

  VmaAllocator allocator = VK_NULL_HANDLE;

  VkExtent3D windowExtent;
  VkFormat colorImageFormat = VK_FORMAT_R8G8B8A8_SRGB;
  VkImage colorImage = VK_NULL_HANDLE;
  VmaAllocation colorImageAllocation;
  VkImageView colorImageView = VK_NULL_HANDLE;

  VkShaderModule vertexShader = VK_NULL_HANDLE;
  VkShaderModule fragmentShader = VK_NULL_HANDLE;

  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  VkPipeline pipeline = VK_NULL_HANDLE;

  VkCommandPool commandPool = VK_NULL_HANDLE;
  VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

  void InitVolk();
  void CreateInstance();
  void LoadInstanceFunctions() { volkLoadInstance(instance); }
  void CreateDebugMessenger();
  void PickPhysicalDevice();
  void FindQueueFamily();
  void CreateDevice();
  void LoadDeviceFunctions() { volkLoadDevice(device); }
  void CreateAllocator();
  void CreateImage();
  void CreateImageView();
  void CreateShaderModules();
  void CreatePipelineLayout();
  void CreatePipeline();
  void CreateCommandPool();
  void CreateCommandBuffer();

public:
  void Init();
  void Destroy();
};

} // namespace core::vk
