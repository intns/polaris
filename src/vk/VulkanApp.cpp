#include <vk/VulkanApp.hpp>
#include <vk/Shaders.hpp>
#include <vulkan/vulkan_core.h>
#include <algorithm>
#include <ranges>
#include <unordered_set>
#include <utility>
#include <cstdint>
#include <vector>

namespace polaris::vk {
namespace {
  struct SwapchainDetails {
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };

  // @return (first: graphics queue family index, second: presentation queue family index)
  std::optional<std::pair<uint32_t, uint32_t>> findQueueFamily(VkPhysicalDevice device, VkSurfaceKHR surface);
  uint32_t getDeviceScore(VkPhysicalDevice device);
  bool checkDeviceExtensions(VkPhysicalDevice device);
  SwapchainDetails querySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
  bool isSwapchainAdequate(VkPhysicalDevice device, VkSurfaceKHR surface);
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
  std::expected<VkShaderModule, std::string> createShaderModule(VkDevice device, std::span<const uint8_t> code);
}  // namespace

std::optional<GLFWwindow*> VulkanApp::initWindow(int width, int height, const char* title) {
  if (glfwInit() == GLFW_FALSE) {
    return std::nullopt;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  window = glfwCreateWindow(width, height, title, nullptr, nullptr);

  if (window == nullptr) {
    glfwTerminate();
    return std::nullopt;
  }

  return window;
}

VkAppExpected VulkanApp::init() {
  if (auto r = createInstance(); !r) { return r; }
  if (bValidationLayers) { setupDebugMessenger(); }
  if (auto r = createSurface(); !r) { return r; }
  if (auto r = pickPhysicalDevice(); !r) { return r; }
  if (auto r = createDevice(); !r) { return r; }
  if (auto r = createSwapchain(); !r) { return r; }
  if (auto r = createImageViews(); !r) { return r; }
  if (auto r = createRenderPass(); !r) { return r; }
  if (auto r = createPipeline(); !r) { return r; }
  if (auto r = createFramebuffers(); !r) { return r; }
  if (auto r = createCommandPool(); !r) { return r; }
  if (auto r = createCommandBuffers(); !r) { return r; }
  if (auto r = createSyncObjects(); !r) { return r; }
  return {};
}

VkAppExpected VulkanApp::render() {
  vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

  uint32_t imageIndex = 0;
  VkResult nextImageRes = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
  if (nextImageRes == VK_ERROR_OUT_OF_DATE_KHR) {
    if (auto r = recreateSwapchain(); !r) { return r; }
    return {};
  }
  if (nextImageRes != VK_SUCCESS && nextImageRes != VK_SUBOPTIMAL_KHR) {
    return std::unexpected("Could not acquire swapchain image");
  }

  vkResetFences(device, 1, &inFlightFences[currentFrame]);

  vkResetCommandBuffer(commandBuffers[currentFrame], 0);
  if (auto err = recordCommandBuffer(commandBuffers[currentFrame], imageIndex); !err.has_value()) {
    std::cerr << err.error() << '\n';
  }

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  auto waitSemaphores = std::to_array({imageAvailableSemaphores[currentFrame]});
  auto waitStages = std::to_array<VkPipelineStageFlags>({VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT});
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores.data();
  submitInfo.pWaitDstStageMask = waitStages.data();
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

  auto signalSemaphores = std::to_array({renderFinishedSemaphores[currentFrame]});
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores.data();

  if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
    return std::unexpected("Could not submit draw command buffer");
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores.data();

  auto swapchains = std::to_array({swapchain});
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapchains.data();
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr;

  VkResult queuePresentRes = vkQueuePresentKHR(presentQueue, &presentInfo);
  if (queuePresentRes == VK_ERROR_OUT_OF_DATE_KHR || queuePresentRes == VK_SUBOPTIMAL_KHR) {
    if (auto r = recreateSwapchain(); !r) { std::cerr << r.error() << '\n'; }
  }
  if (queuePresentRes != VK_SUCCESS) {
    return std::unexpected("Could not present swapchain image");
  }

  currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
  return {};
}

void VulkanApp::cleanup() {
  if (instance == VK_NULL_HANDLE) {
    return;
  }

  cleanupSwapchain();
  vkDestroyPipeline(device, pipeline, nullptr);
  vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
  vkDestroyRenderPass(device, renderPass, nullptr);
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
    vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
    vkDestroyFence(device, inFlightFences[i], nullptr);
  }
  vkDestroyCommandPool(device, commandPool, nullptr);
  vkDestroyDevice(device, nullptr);
  if (bValidationLayers && debugMessenger != VK_NULL_HANDLE) {
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    if (func != nullptr) { func(instance, debugMessenger, nullptr); }
  }
  vkDestroySurfaceKHR(instance, surface, nullptr);
  vkDestroyInstance(instance, nullptr);

  glfwDestroyWindow(window);
  glfwTerminate();
}

VkAppExpected VulkanApp::createInstance() {
  using std::ranges::views::transform;

  VkApplicationInfo appInfo {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "polaris";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_4;

  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensionNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  std::vector<std::string_view> extensions;

  if (glfwExtensionNames != nullptr) {
    extensions.assign(glfwExtensionNames, glfwExtensionNames + glfwExtensionCount); // NOLINT
  }
  else {
    if (glfwVulkanSupported() == GLFW_FALSE) {
      return std::unexpected("Could not get Vulkan loader");
    }
    std::cerr << "Expected to get some required extensions, but did not\n";
  }

  if (bValidationLayers && std::ranges::find(extensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == extensions.end()) {
    extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  VkInstanceCreateInfo createInfo {};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledLayerCount = 0;
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  auto extensionPtrs = extensions | transform([](std::string_view v) { return v.data(); }) | std::ranges::to<std::vector>();
  createInfo.ppEnabledExtensionNames = extensionPtrs.data();

  if (bValidationLayers) {
    createInfo.enabledLayerCount = validationLayers.size();
    createInfo.ppEnabledLayerNames = validationLayers.data();
  }

  if (auto err = vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    return std::unexpected(std::format("Could not create a vulkan instance: {}", err));
  }
  return {};
}

VkAppExpected VulkanApp::createSurface() {
  if (auto err = glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
    return std::unexpected(std::format("Could not create window surface: {}", err));
  }
  return {};
}

VkAppExpected VulkanApp::pickPhysicalDevice() {
  uint32_t count = 0;
  vkEnumeratePhysicalDevices(instance, &count, nullptr);
  if (count == 0) {
    return std::unexpected("Could not find vulkan devices");
  }

  std::vector<VkPhysicalDevice> devices(count);
  vkEnumeratePhysicalDevices(instance, &count, devices.data());

  std::vector<std::pair<uint32_t, VkPhysicalDevice>> suitable_devices;
  for (const auto& d : devices) {
    uint32_t device_score = getDeviceScore(d);
    if (device_score == 0 || !findQueueFamily(d, surface).has_value() || !checkDeviceExtensions(d) || !isSwapchainAdequate(d, surface)) {
      continue;
    }
    suitable_devices.emplace_back(device_score, d);
  }

  if (suitable_devices.empty()) {
    return std::unexpected("No suitable devices could be found");
  }
  physicalDevice = std::ranges::max_element(suitable_devices, [](const auto& left, const auto& right) { return left.first < right.first; })->second;
  return {};
}

VkAppExpected VulkanApp::createDevice() {
  auto queueFamilyIndex = findQueueFamily(physicalDevice, surface);
  auto graphicsQueueFamilyIndex = queueFamilyIndex.value().first;
  auto presentQueueFamilyIndex = queueFamilyIndex.value().second;
  float queuePriority = 1.0f;

  VkDeviceQueueCreateInfo graphicsQueueCreateInfo {};
  graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  graphicsQueueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
  graphicsQueueCreateInfo.queueCount = 1;
  graphicsQueueCreateInfo.pQueuePriorities = &queuePriority;

  VkDeviceQueueCreateInfo presentQueueCreateInfo {};
  presentQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  presentQueueCreateInfo.queueFamilyIndex = presentQueueFamilyIndex;
  presentQueueCreateInfo.queueCount = 1;
  presentQueueCreateInfo.pQueuePriorities = &queuePriority;

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = {graphicsQueueCreateInfo};
  if (graphicsQueueFamilyIndex != presentQueueFamilyIndex) {
    queueCreateInfos.push_back(presentQueueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures{};
  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.pEnabledFeatures = &deviceFeatures;
  createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();
  createInfo.enabledLayerCount = 0;

  if (bValidationLayers) {
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  }

  if (auto err = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
    return std::unexpected(std::format("Could not create logical device: {}", err));
  }
  vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);
  if (graphicsQueueFamilyIndex != presentQueueFamilyIndex) {
    vkGetDeviceQueue(device, presentQueueFamilyIndex, 0, &presentQueue);
  } else {
    presentQueue = graphicsQueue;
  }
  return {};
}

VkAppExpected VulkanApp::createSwapchain() {
  SwapchainDetails details = querySwapchainSupport(physicalDevice, surface);
  auto surfaceFormat = chooseSwapSurfaceFormat(details.formats);
  VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
  auto extent = chooseSwapExtent(details.capabilities, window);
  uint32_t imageCount = details.capabilities.minImageCount + 1;

  if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount) {
    imageCount = details.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  createInfo.preTransform = details.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE; // TODO: DANGER!
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  auto queueFamilyIndex = findQueueFamily(physicalDevice, surface).value();
  auto indices = std::to_array({queueFamilyIndex.first, queueFamilyIndex.second});
  if (queueFamilyIndex.first != queueFamilyIndex.second) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = indices.data();
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
  }

  if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
    return std::unexpected("Could not create swapchain");
  }

  vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
  images.resize(imageCount);
  vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data());
  imageFormat = surfaceFormat.format;
  swapchainExtent = extent;

  return {};
}

VkAppExpected VulkanApp::createImageViews() {
  imageViews.resize(images.size());
  for (size_t i = 0; i < images.size(); i++) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = images[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = imageFormat;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device, &createInfo, nullptr, &imageViews[i]) != VK_SUCCESS) {
      return std::unexpected("Could not create image views");
    }
  }
  return {};
}

VkAppExpected VulkanApp::createRenderPass() {
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = imageFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
    return std::unexpected("Could not create render pass");
  }
  return {};
}

VkAppExpected VulkanApp::createPipeline() {
  auto vertShaderCode = ShaderManager::getVert("triangle").value();
  auto fragShaderCode = ShaderManager::getFrag("triangle").value();

  auto vertShaderModule = createShaderModule(device, vertShaderCode);
  auto fragShaderModule = createShaderModule(device, fragShaderCode);
  if (!vertShaderModule.has_value()) {
    return std::unexpected(std::format("vertex shader: {}", vertShaderModule.error()));
  }
  if (!fragShaderModule.has_value()) {
    return std::unexpected(std::format("fragment shader: {}", fragShaderModule.error()));
  }

  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule.value();
  vertShaderStageInfo.pName = "main";
  VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
  fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule.value();
  fragShaderStageInfo.pName = "main";
  auto shaderStages = std::to_array({vertShaderStageInfo, fragShaderStageInfo});

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 0;
  vertexInputInfo.pVertexBindingDescriptions = nullptr;
  vertexInputInfo.vertexAttributeDescriptionCount = 0;
  vertexInputInfo.pVertexAttributeDescriptions = nullptr;

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  std::vector<VkDynamicState> dynamicStates = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
  };

  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicState.pDynamicStates = dynamicStates.data();

  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.scissorCount = 1;

  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0f;
  rasterizer.depthBiasClamp = 0.0f;
  rasterizer.depthBiasSlopeFactor = 0.0f;

  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading = 1.0f;
  multisampling.pSampleMask = nullptr;
  multisampling.alphaToCoverageEnable = VK_FALSE;
  multisampling.alphaToOneEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT; // NOLINT
  colorBlendAttachment.blendEnable = VK_FALSE;
  // tldr: output = blendEnable ? (srcBlendFactor * new) <blendOp> (dstBlendFactor * old) : new
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;

  if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
      return std::unexpected("Could not create pipeline layout");
  }

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages.data();
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = nullptr;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.layout = pipelineLayout;
  pipelineInfo.renderPass = renderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineInfo.basePipelineIndex = -1;

  if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
    return std::unexpected("Coud not create graphics pipeline");
  }

  vkDestroyShaderModule(device, fragShaderModule.value(), nullptr);
  vkDestroyShaderModule(device, vertShaderModule.value(), nullptr);
  return {};
}

VkAppExpected VulkanApp::createFramebuffers() {
  framebuffers.resize(imageViews.size());
  for (size_t i = 0; i < framebuffers.size(); i++) {
    auto attachments = std::to_array({
      imageViews[i]
    });

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = swapchainExtent.width;
    framebufferInfo.height = swapchainExtent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
        return std::unexpected(std::format("Could not create framebuffer {}", i));
    }
  }
  return {};
}

VkAppExpected VulkanApp::createCommandPool() {
  auto queueFamilyIndices = findQueueFamily(physicalDevice, surface);

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = queueFamilyIndices.value().first;

  if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
    return std::unexpected("Could not create command pool");
  }
  return {};
}

VkAppExpected VulkanApp::createCommandBuffers() {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

  if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
      return std::unexpected("Could not create command buffers");
  }
  return {};
}

VkAppExpected VulkanApp::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;
  beginInfo.pInheritanceInfo = nullptr;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
      return std::unexpected("Could not begin recording command buffer");
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = renderPass;
  renderPassInfo.framebuffer = framebuffers[imageIndex];
  renderPassInfo.renderArea.offset = {.x=0, .y=0};
  renderPassInfo.renderArea.extent = swapchainExtent;

  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(swapchainExtent.width);
  viewport.height = static_cast<float>(swapchainExtent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {.x=0, .y=0};
  scissor.extent = swapchainExtent;
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  vkCmdDraw(commandBuffer, 3, 1, 0, 0);
  vkCmdEndRenderPass(commandBuffer);
  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    return std::unexpected("Could not record command buffer");
  }
  return {};
}

VkAppExpected VulkanApp::createSyncObjects() {
  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS
     || vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS
     || vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
    {
      return std::unexpected("Could not create semaphores");
    }
  }
  return {};
}

VkAppExpected VulkanApp::recreateSwapchain() {
  int width = 0;
  int height = 0;
  glfwGetFramebufferSize(window, &width, &height);
  while (width == 0 || height == 0) {
    glfwGetFramebufferSize(window, &width, &height);
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(device);

  if (auto r = createSwapchain(); !r) { return r; }
  if (auto r = createImageViews(); !r) { return r; }
  if (auto r = createFramebuffers(); !r) { return r; }
  return {};
}

void VulkanApp::cleanupSwapchain() {
  for (auto* fb : framebuffers) {
    vkDestroyFramebuffer(device, fb, nullptr);
  }
  for (auto* imageView : imageViews) {
    vkDestroyImageView(device, imageView, nullptr);
  }
  vkDestroySwapchainKHR(device, swapchain, nullptr);
}

void VulkanApp::setupDebugMessenger() {
  if (!bValidationLayers) { return; }

  // NOLINTBEGIN(hicpp-signed-bitwise)
  VkDebugUtilsMessengerCreateInfoEXT createInfo {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
    | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
    | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
    | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
    | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
  createInfo.pUserData = nullptr;
  // NOLINTEND(hicpp-signed-bitwise)

  auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
  if (func == nullptr) {
    std::cerr << "Could not get vkCreateDebugUtilsMessengerEXT from instance\n";
    return;
  }

  if (auto err = func(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
    std::cerr << "Could not create debug messenger: " << err << '\n';
    return;
  }
}

namespace {
// @return (first: graphics queue family index, second: presentation queue family index)
std::optional<std::pair<uint32_t, uint32_t>> findQueueFamily(VkPhysicalDevice device, VkSurfaceKHR surface) {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::optional<uint32_t> graphicsIndex = std::nullopt;
  std::optional<uint32_t> presentIndex = std::nullopt;
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
  int i = 0;
  for (const auto& queueFamily : queueFamilies) {
    // NOLINTBEGIN(readability-implicit-bool-conversion)
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphicsIndex = i;
    }

    VkBool32 canPresent = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &canPresent);
    if (canPresent) {
      presentIndex = i;
    }
    // NOLINTEND(readability-implicit-bool-conversion)

    if (graphicsIndex.has_value() && presentIndex.has_value() && graphicsIndex == presentIndex) {
      return std::make_pair(graphicsIndex.value(), presentIndex.value());
    }
  }

  if (graphicsIndex == std::nullopt) {
    return std::nullopt;
  }
  if (presentIndex == std::nullopt) {
    return std::nullopt;
  }

  return std::make_pair(graphicsIndex.value(), presentIndex.value());
}

uint32_t getDeviceScore(VkPhysicalDevice device) {
  VkPhysicalDeviceProperties properties;
  VkPhysicalDeviceFeatures features;
  vkGetPhysicalDeviceProperties(device, &properties);
  vkGetPhysicalDeviceFeatures(device, &features);

  uint32_t score = 0;
  switch (properties.deviceType) {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
      score += 100000;
      break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
      score += 50000;
      break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
      score += 20000;
      break;
    default:
      break;
  }

  score += properties.limits.maxImageDimension2D;
  if (!features.geometryShader) { // NOLINT
    return 0;
  }

  return score;
};

bool checkDeviceExtensions(VkPhysicalDevice device) {
  uint32_t extensionCount = 0;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

  using std::ranges::views::transform;
  auto availableNames = availableExtensions
    | transform(&VkExtensionProperties::extensionName)
    | std::ranges::to<std::unordered_set<std::string_view>>();

  return std::ranges::all_of(deviceExtensions, [&](std::string_view x) { return availableNames.contains(x); });
};

SwapchainDetails querySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
  SwapchainDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

  uint32_t formatCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
  }

  uint32_t presentModeCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
  }

  return details;
}

bool isSwapchainAdequate(VkPhysicalDevice device, VkSurfaceKHR surface) {
  auto details = querySwapchainSupport(device, surface);
  return !details.formats.empty() && !details.presentModes.empty();
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
  for (const auto& availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  return availableFormats[0];
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  }

  int width = 0;
  int height = 0;
  glfwGetFramebufferSize(window, &width, &height);

  VkExtent2D actualExtent = {
    static_cast<uint32_t>(width),
    static_cast<uint32_t>(height)
  };

  actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
  actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

  return actualExtent;
}

std::expected<VkShaderModule, std::string> createShaderModule(VkDevice device, std::span<const uint8_t> code) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

  VkShaderModule shaderModule = nullptr;
  if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
      return std::unexpected("Could not create shader module");
  }
  return shaderModule;
}

}  // namespace
}  // namespace polaris::vk
