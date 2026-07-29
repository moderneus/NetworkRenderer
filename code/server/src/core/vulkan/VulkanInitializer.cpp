#include "core/vulkan/VulkanInitializer.hpp"
#include "utils/Defines.hpp"

#include <array>
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

  if (vulkanHelper.CheckInstanceExtensionsSupport(instanceExtensionNames)) {
    info.enabledExtensionCount =
        static_cast<std::uint32_t>(instanceExtensionNames.size());
    info.ppEnabledExtensionNames = instanceExtensionNames.data();
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

  for (const auto &physicalDevice : physicalDevices)
    candidates.insert(std::make_pair(
        vulkanHelper.RatePhysicalDevice(physicalDevice), physicalDevice));

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

  if (vulkanHelper.CheckDeviceExtensionsSupport(
          physicalDevice, deviceExtensionNames)) {
    dynamicRenderingFeature = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
      .dynamicRendering = VK_TRUE,
    };
  }

  VkDeviceCreateInfo info{
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pNext = &dynamicRenderingFeature,
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

void core::vk::VulkanInitializer::CreateImage() {
  /* this will be replaced by the client's window extent */
  windowExtent = { 800, 600, 1 };

  VkImageCreateInfo imageInfo{
    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    .imageType = VK_IMAGE_TYPE_2D,
    .format = VK_FORMAT_UNDEFINED,
    .extent = windowExtent,
    .mipLevels = 1,
    .arrayLayers = 1,
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .tiling = VK_IMAGE_TILING_OPTIMAL,
    .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 1,
    .pQueueFamilyIndices = &gfxQueueFamilyIndex,
    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
  };

  if (vulkanHelper.CheckImageFormatSupport(colorImageFormat, physicalDevice))
    imageInfo.format = colorImageFormat;
  else
    fmt::print(fmt::fg(fmt::color::dark_red),
        "[VULKAN] Failed to create an image: "
        "required format is not supported\n");

  VmaAllocationCreateInfo allocationInfo{
    .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT |
             VMA_ALLOCATION_CREATE_MAPPED_BIT,
    .usage = VMA_MEMORY_USAGE_GPU_ONLY,
    .requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    .preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    .memoryTypeBits = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    .priority = 1.0,
  };

  vmaCreateImage(allocator, &imageInfo, &allocationInfo, &colorImage,
      &colorImageAllocation, nullptr);

  deletionQueue.Push([this]() {
    vmaDestroyImage(allocator, colorImage, colorImageAllocation);
  });
}

void core::vk::VulkanInitializer::CreateImageView() {
  VkImageViewCreateInfo info{
    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    .image = colorImage,
    .viewType = VK_IMAGE_VIEW_TYPE_2D,
    .format = colorImageFormat,
    .components = {
      VK_COMPONENT_SWIZZLE_IDENTITY,
      VK_COMPONENT_SWIZZLE_IDENTITY,
      VK_COMPONENT_SWIZZLE_IDENTITY,
      VK_COMPONENT_SWIZZLE_IDENTITY,
    },
    .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1, },
  };

  VK_CHECK(vkCreateImageView(device, &info, nullptr, &colorImageView));

  deletionQueue.Push(
      [this]() { vkDestroyImageView(device, colorImageView, nullptr); });
}

void core::vk::VulkanInitializer::CreateShaderModules() {
  VkShaderModuleCreateInfo vertexShaderInfo{
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .codeSize = static_cast<std::size_t>(vertexShaderCode.size()),
    .pCode = reinterpret_cast<const std::uint32_t *>(vertexShaderCode.data()),
  };

  VkShaderModuleCreateInfo fragmentShaderInfo{
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .codeSize = static_cast<std::size_t>(fragmentShaderCode.size()),
    .pCode = reinterpret_cast<const std::uint32_t *>(fragmentShaderCode.data()),
  };

  VK_CHECK(
      vkCreateShaderModule(device, &vertexShaderInfo, nullptr, &vertexShader));
  VK_CHECK(vkCreateShaderModule(
      device, &fragmentShaderInfo, nullptr, &fragmentShader));

  deletionQueue.Push(
      [this]() { vkDestroyShaderModule(device, vertexShader, nullptr); });
  deletionQueue.Push(
      [this]() { vkDestroyShaderModule(device, fragmentShader, nullptr); });
}

void core::vk::VulkanInitializer::CreatePipelineLayout() {
  VkPipelineLayoutCreateInfo info{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .setLayoutCount = 0,
    .pSetLayouts = nullptr,
    .pushConstantRangeCount = 0,
    .pPushConstantRanges = nullptr,
  };

  VK_CHECK(vkCreatePipelineLayout(device, &info, nullptr, &pipelineLayout));

  deletionQueue.Push(
      [this]() { vkDestroyPipelineLayout(device, pipelineLayout, nullptr); });
}

void core::vk::VulkanInitializer::CreatePipeline() {
  std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};
  shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
  shaderStages[0].module = vertexShader;
  shaderStages[0].pName = "main";

  shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  shaderStages[1].module = fragmentShader;
  shaderStages[1].pName = "main";

  VkVertexInputBindingDescription vertexBindingDescription{
    .binding = 0,
    .stride = sizeof(VulkanHelper::Vertex),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
  };

  std::array<VkVertexInputAttributeDescription, 2> vertexAttributeDescription{};
  vertexAttributeDescription[0].binding = 0;
  vertexAttributeDescription[0].location = 0;
  vertexAttributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  vertexAttributeDescription[0].offset =
      offsetof(VulkanHelper::Vertex, position);

  vertexAttributeDescription[1].binding = 0;
  vertexAttributeDescription[1].location = 1;
  vertexAttributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  vertexAttributeDescription[1].offset = offsetof(VulkanHelper::Vertex, color);

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    .vertexBindingDescriptionCount = 1,
    .pVertexBindingDescriptions = &vertexBindingDescription,
    .vertexAttributeDescriptionCount =
        static_cast<std::uint32_t>(vertexAttributeDescription.size()),
    .pVertexAttributeDescriptions = vertexAttributeDescription.data(),
  };

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    .primitiveRestartEnable = VK_FALSE,
  };

  VkViewport viewport{
    .x = 0.0f,
    .y = 0.0f,
    .width = static_cast<float>(windowExtent.width),
    .height = static_cast<float>(windowExtent.height),
    .minDepth = 0.0f,
    .maxDepth = 1.0f,
  };

  VkRect2D scissor{
    .offset = { 0, 0 },
    .extent = { windowExtent.width, windowExtent.height },
  };

  VkPipelineViewportStateCreateInfo viewportInfo{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .viewportCount = 1,
    .pViewports = &viewport,
    .scissorCount = 1,
    .pScissors = &scissor,
  };

  VkPipelineRasterizationStateCreateInfo rasterizationInfo{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .depthClampEnable = VK_FALSE,
    .rasterizerDiscardEnable = VK_FALSE,
    .polygonMode = VK_POLYGON_MODE_FILL,
    .cullMode = VK_CULL_MODE_BACK_BIT,
    .frontFace = VK_FRONT_FACE_CLOCKWISE,
    .depthBiasEnable = VK_FALSE,
    .lineWidth = 1.0f,
  };

  VkPipelineMultisampleStateCreateInfo multisampleInfo{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    .sampleShadingEnable = VK_FALSE,
  };

  VkPipelineColorBlendAttachmentState colorBlendAttachment{
    .blendEnable = VK_FALSE,
    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
  };

  VkPipelineColorBlendStateCreateInfo colorBlendInfo{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .logicOpEnable = VK_FALSE,
    .attachmentCount = 1,
    .pAttachments = &colorBlendAttachment,
  };

  std::array<VkDynamicState, 2> dynamicStates{ VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR };

  VkPipelineDynamicStateCreateInfo dynamicStateInfo{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    .dynamicStateCount = static_cast<std::uint32_t>(dynamicStates.size()),
    .pDynamicStates = dynamicStates.data(),
  };

  VkPipelineRenderingCreateInfo renderingInfo{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
    .colorAttachmentCount = 1,
    .pColorAttachmentFormats = &colorImageFormat,
    .depthAttachmentFormat = VK_FORMAT_UNDEFINED,
    .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
  };

  VkGraphicsPipelineCreateInfo info{
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .pNext = &renderingInfo,
    .stageCount = static_cast<std::uint32_t>(shaderStages.size()),
    .pStages = shaderStages.data(),
    .pVertexInputState = &vertexInputInfo,
    .pInputAssemblyState = &inputAssemblyInfo,
    .pViewportState = &viewportInfo,
    .pRasterizationState = &rasterizationInfo,
    .pMultisampleState = &multisampleInfo,
    .pColorBlendState = &colorBlendInfo,
    .pDynamicState = &dynamicStateInfo,
    .layout = pipelineLayout,
    .renderPass = VK_NULL_HANDLE,
    .subpass = 0,
  };

  VK_CHECK(vkCreateGraphicsPipelines(
      device, VK_NULL_HANDLE, 1, &info, nullptr, &pipeline));

  deletionQueue.Push(
      [this]() { vkDestroyPipeline(device, pipeline, nullptr); });
}

void core::vk::VulkanInitializer::CreateCommandPool() {
  VkCommandPoolCreateInfo info{
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
    .queueFamilyIndex = gfxQueueFamilyIndex,
  };

  VK_CHECK(vkCreateCommandPool(device, &info, nullptr, &commandPool));

  deletionQueue.Push(
      [this]() { vkDestroyCommandPool(device, commandPool, nullptr); });
}

void core::vk::VulkanInitializer::CreateCommandBuffer() {
  VkCommandBufferAllocateInfo info{
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .commandPool = commandPool,
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = 1,
  };

  VK_CHECK(vkAllocateCommandBuffers(device, &info, &commandBuffer));
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
  CreateImage();
  CreateImageView();
  CreateShaderModules();
  CreatePipelineLayout();
  CreatePipeline();
  CreateCommandPool();
  CreateCommandBuffer();
}

void core::vk::VulkanInitializer::Destroy() { deletionQueue.CleanUp(); }
