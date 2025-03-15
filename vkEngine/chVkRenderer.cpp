#include "chVkRenderer.hpp"


#include <stdexcept>
#include <array>
#include <cassert>

namespace chVk
{
	ChVkRenderer::ChVkRenderer(ChVkWindow& window, ChVkDevice& device) : _chVkWindow(window), _chVkDevice(device)
	{
		RecreateSwapChain();
		CreateCommandBuffers();
	}

	ChVkRenderer::~ChVkRenderer()
	{
		FreeCommandBuffers();
	}

	void ChVkRenderer::CreateCommandBuffers()
	{
		assert(_chVkSwapChain != nullptr && "Cannot Create Pipeline befor SwapChain");
		
		_commandBuffers.resize(_chVkSwapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = _chVkDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());

		if (vkAllocateCommandBuffers(_chVkDevice.device(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers");
		}
	}

	void ChVkRenderer::FreeCommandBuffers()
	{
		vkFreeCommandBuffers(_chVkDevice.device(), _chVkDevice.getCommandPool(), static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
		_commandBuffers.clear();
	}

	void ChVkRenderer::RecreateSwapChain()
	{
		auto extent = _chVkWindow.GetExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = _chVkWindow.GetExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(_chVkDevice.device());

		if (_chVkSwapChain == nullptr)
		{
			_chVkSwapChain = std::make_unique<ChVkSwapChain>(_chVkDevice, extent);
		}
		else
		{
			_chVkSwapChain = std::make_unique<ChVkSwapChain>(_chVkDevice, extent, std::move(_chVkSwapChain));
			if (_chVkSwapChain->imageCount() != _commandBuffers.size())
			{
				FreeCommandBuffers();
				CreateCommandBuffers();
			}
		}
		//CreatePipeline();
	}

	VkCommandBuffer ChVkRenderer::BeginFrame()
	{
		assert(!_isFrameStarted && "Cannot call BeginFrame while already in progress");

		VkResult result = _chVkSwapChain->acquireNextImage(&_currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return nullptr;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		_isFrameStarted = true;

		VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}
		return commandBuffer;
	}

	void ChVkRenderer::EndFrame()
	{
		assert(_isFrameStarted && "Cannot call EndFrame while frame is not in progress");
		VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}

		VkResult result = _chVkSwapChain->SubmitCommandBuffers(&commandBuffer, &_currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _chVkWindow.WasWindowResized())
		{
			_chVkWindow.ResetWindowResizedFlag();
			RecreateSwapChain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}

		_isFrameStarted = false;

	}

	void ChVkRenderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(_isFrameStarted && 
			"Cannot call BeginSwapChainRenderPass if frame is not in progress");
		assert(commandBuffer == GetCurrentCommandBuffer() && 
			"Can only call BeginSwapChainRenderPass for the currently active command buffer");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = _chVkSwapChain->getRenderPass();
		renderPassInfo.framebuffer = _chVkSwapChain->getFrameBuffer(_currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = _chVkSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(_chVkSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(_chVkSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0,0}, _chVkSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void ChVkRenderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(_isFrameStarted &&
			"Cannot call EndSwapChainRenderPass if frame is not in progress");
		assert(commandBuffer == GetCurrentCommandBuffer() &&
			"Can only call EndSwapChainRenderPass for the currently active command buffer");
		vkCmdEndRenderPass(commandBuffer);

	}
}; // namespace ckVk