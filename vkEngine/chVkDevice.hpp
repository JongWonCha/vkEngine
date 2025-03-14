#pragma once

#include "chVkWindow.hpp"

// std lib headers
#include <string>
#include <vector>

namespace chVk {

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
  uint32_t graphicsFamily;
  uint32_t presentFamily;
  bool graphicsFamilyHasValue = false;
  bool presentFamilyHasValue = false;
  bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
};

class ChVkDevice {
 public:
#ifdef NDEBUG
  const bool enableValidationLayers = false;
#else
  const bool enableValidationLayers = true;
#endif

  ChVkDevice(ChVkWindow &window);
  ~ChVkDevice();

  // Not copyable or movable
  ChVkDevice(const ChVkDevice &) = delete;
  ChVkDevice &operator=(const ChVkDevice &) = delete;
  ChVkDevice(ChVkDevice &&) = delete;
  ChVkDevice &operator=(ChVkDevice &&) = delete;

  VkCommandPool getCommandPool() { return _commandPool; }
  VkDevice device() { return _device; }
  VkSurfaceKHR surface() { return _surface; }
  VkQueue graphicsQueue() { return _graphicsQueue; }
  VkQueue presentQueue() { return _presentQueue; }

  SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(_physicalDevice); }
  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
  QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(_physicalDevice); }
  VkFormat findSupportedFormat(
      const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

  // Buffer Helper Functions
  void createBuffer(
      VkDeviceSize size,
      VkBufferUsageFlags usage,
      VkMemoryPropertyFlags properties,
      VkBuffer &buffer,
      VkDeviceMemory &bufferMemory);
  VkCommandBuffer beginSingleTimeCommands();
  void endSingleTimeCommands(VkCommandBuffer commandBuffer);
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  void copyBufferToImage(
      VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

  void createImageWithInfo(
      const VkImageCreateInfo &imageInfo,
      VkMemoryPropertyFlags properties,
      VkImage &image,
      VkDeviceMemory &imageMemory);

  VkPhysicalDeviceProperties properties;

 private:
  void createInstance();
  void setupDebugMessenger();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();
  void createCommandPool();

  // helper functions
  bool isDeviceSuitable(VkPhysicalDevice device);
  std::vector<const char *> getRequiredExtensions();
  bool checkValidationLayerSupport();
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
  void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
  void hasGflwRequiredInstanceExtensions();
  bool checkDeviceExtensionSupport(VkPhysicalDevice device);
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

  VkInstance _instance;
  VkDebugUtilsMessengerEXT _debugMessenger;
  VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
  ChVkWindow &window;
  VkCommandPool _commandPool;

  VkDevice      _device;
  VkSurfaceKHR  _surface;
  VkQueue       _graphicsQueue;
  VkQueue       _presentQueue;

  const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
  const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};

}  // namespace chVk