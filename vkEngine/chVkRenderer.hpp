#pragma once

#include "chVkWindow.hpp"
#include "chVkDevice.hpp"
#include "chVkSwapChain.hpp"

// std
#include <memory>
#include <vector>
#include <cassert>

namespace chVk
{
	class ChVkRenderer
    {
    public:
        ChVkRenderer(ChVkWindow& window, ChVkDevice& device);
        ~ChVkRenderer();

        ChVkRenderer(const ChVkRenderer&) = delete;
        ChVkRenderer& operator=(const ChVkRenderer&) = delete;

        VkCommandBuffer                 BeginFrame();
        void                            EndFrame();
		void                            BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void                            EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

		VkRenderPass					GetSwapChainRenderPass() const { return _chVkSwapChain->getRenderPass(); }
		bool                            IsFrameInProgress() const { return _isFrameStarted; }
		VkCommandBuffer                 GetCurrentCommandBuffer() const
        { 
			assert(_isFrameStarted && "Cannot Get CommandBuffer When Frame Not In Progress");
            return _commandBuffers[_currentFrameIndex]; 
        }

		int GetFrameIndex() const
        { 
			assert(_isFrameStarted && "Cannot Get Frame Index When Frame Not In Progress");
            return _currentFrameIndex;
        }

    private:
        void                            CreateCommandBuffers();
        void                            FreeCommandBuffers();
        void                            RecreateSwapChain();

        ChVkWindow&                     _chVkWindow;
        ChVkDevice&                     _chVkDevice;
        std::unique_ptr<ChVkSwapChain>  _chVkSwapChain;
        std::vector<VkCommandBuffer>    _commandBuffers;

        uint32_t                        _currentImageIndex{ 0 };
		int                             _currentFrameIndex{ 0 };
        bool                            _isFrameStarted{ false };
    };
}