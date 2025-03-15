#pragma once

#include "chVkdevice.hpp"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <string>
#include <vector>
#include <memory>

namespace chVk {

class ChVkSwapChain {
 public:
  static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

  ChVkSwapChain(ChVkDevice &deviceRef, VkExtent2D windowExtent);
  ChVkSwapChain(ChVkDevice& deviceRef, VkExtent2D windowExtent, std::shared_ptr<ChVkSwapChain> previous);
  ~ChVkSwapChain();

  ChVkSwapChain(const ChVkSwapChain &) = delete;
  ChVkSwapChain& operator=(const ChVkSwapChain &) = delete;

  VkFramebuffer getFrameBuffer(int index) { return _swapChainFramebuffers[index]; }
  VkRenderPass getRenderPass() { return _renderPass; }
  VkImageView getImageView(int index) { return _swapChainImageViews[index]; }
  size_t imageCount() { return _swapChainImages.size(); }
  VkFormat getSwapChainImageFormat() { return _swapChainImageFormat; }
  VkExtent2D getSwapChainExtent() { return _swapChainExtent; }
  uint32_t width() { return _swapChainExtent.width; }
  uint32_t height() { return _swapChainExtent.height; }

  float extentAspectRatio() {
    return static_cast<float>(_swapChainExtent.width) / static_cast<float>(_swapChainExtent.height);
  }
  VkFormat findDepthFormat();

  VkResult acquireNextImage(uint32_t *imageIndex);
  VkResult SubmitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

  bool CompareSwapFormats(const ChVkSwapChain& swapChain) const
  {
	  return    _swapChainImageFormat == swapChain._swapChainImageFormat &&
		        _swapChainDepthFormat == swapChain._swapChainDepthFormat;
  }

 private:
    void Init();
    void createSwapChain();
    void createImageViews();
    void createDepthResources();
    void createRenderPass();
    void createFramebuffers();
    void createSyncObjects();

  // Helper functions
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  VkFormat _swapChainImageFormat;
  VkFormat _swapChainDepthFormat;
  VkExtent2D _swapChainExtent;

  std::vector<VkFramebuffer> _swapChainFramebuffers;
  VkRenderPass _renderPass;

  std::vector<VkImage> _depthImages;
  std::vector<VkDeviceMemory> _depthImageMemorys;
  std::vector<VkImageView> _depthImageViews;
  std::vector<VkImage> _swapChainImages;
  std::vector<VkImageView> _swapChainImageViews;

  ChVkDevice& _device;
  VkExtent2D _windowExtent;

  VkSwapchainKHR _swapChain;
  std::shared_ptr<ChVkSwapChain> _oldSwapChain;

  std::vector<VkSemaphore> _imageAvailableSemaphores;
  std::vector<VkSemaphore> _renderFinishedSemaphores;
  std::vector<VkFence> _inFlightFences;
  std::vector<VkFence> _imagesInFlight;
  size_t _currentFrame = 0;
};

}  // namespace chVk
