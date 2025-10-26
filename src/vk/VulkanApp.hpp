#ifndef POLARIS_VK_HPP
#define POLARIS_VK_HPP

#include <expected>
#include <iostream>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace polaris::vk {

using VkAppExpected = std::expected<void, std::string>;

constexpr auto deviceExtensions = std::to_array({
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
});

#ifdef NDEBUG
constexpr bool bValidationLayers = false;
constexpr std::array<const char*, 0> validationLayers = {};
#else
constexpr bool bValidationLayers = true;
constexpr auto validationLayers = std::to_array({ "VK_LAYER_KHRONOS_validation" });
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( // NOLINT
  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT messageType,
  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
  void* pUserData
) {
  (void)messageSeverity; (void)messageType; (void)pUserData;
  std::cerr << "Vulkan Validation: " << pCallbackData->pMessage << '\n';
  return VK_FALSE;
}

class VulkanApp { // NOLINT
 public:
  VulkanApp() = default;
  ~VulkanApp() = default;

  std::optional<GLFWwindow*> initWindow(int width, int height, const char* title);

  VkAppExpected init();
  VkAppExpected render();
  void cleanup();

  VkDevice getDevice() const { return device; }

  static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
 private:
  GLFWwindow* window = nullptr;

  VkInstance instance = VK_NULL_HANDLE;
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device = VK_NULL_HANDLE;

  VkQueue graphicsQueue = VK_NULL_HANDLE;
  VkQueue presentQueue = VK_NULL_HANDLE;
  VkSwapchainKHR swapchain = VK_NULL_HANDLE;
  VkExtent2D swapchainExtent{};
  VkFormat imageFormat{};
  uint32_t imageCount = 0;
  std::vector<VkImage> images;
  std::vector<VkImageView> imageViews;
  std::vector<VkFramebuffer> framebuffers;

  VkRenderPass renderPass = VK_NULL_HANDLE;
  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  VkPipeline pipeline = VK_NULL_HANDLE;
  VkCommandPool commandPool = VK_NULL_HANDLE;
  std::vector<VkCommandBuffer> commandBuffers{MAX_FRAMES_IN_FLIGHT};

  std::vector<VkSemaphore> imageAvailableSemaphores{MAX_FRAMES_IN_FLIGHT};
  std::vector<VkSemaphore> renderFinishedSemaphores{MAX_FRAMES_IN_FLIGHT};
  std::vector<VkFence> inFlightFences{MAX_FRAMES_IN_FLIGHT};
  uint32_t currentFrame = 0;

  VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

  VkAppExpected createInstance();
  VkAppExpected createSurface();
  VkAppExpected pickPhysicalDevice();
  VkAppExpected createDevice();
  VkAppExpected createSwapchain();
  VkAppExpected createImageViews();
  VkAppExpected createRenderPass();
  VkAppExpected createPipeline();
  VkAppExpected createFramebuffers();
  VkAppExpected createCommandPool();
  VkAppExpected createCommandBuffers();
  VkAppExpected recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
  VkAppExpected createSyncObjects();
  VkAppExpected recreateSwapchain();
  void cleanupSwapchain();
  void setupDebugMessenger();
};
}  // namespace polaris::vk
#endif
