#pragma once

#include "chVkPipeline.hpp"
#include "chVkDevice.hpp"
#include "ChVkGameObject.hpp"

// std
#include <memory>
#include <vector>

namespace chVk
{
	class SimpleRenderSystem
    {
    public:
        SimpleRenderSystem(ChVkDevice& device, VkRenderPass renderPass);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

        void RenderGameObjects(VkCommandBuffer commandBuffer, std::vector<ChVkGameObject>& gameObjects);

    private:
        void CreatePipelineLayout();
        void CreatePipeline(VkRenderPass renderPass);

        ChVkDevice&                     _chVkDevice;

        std::unique_ptr<chVkPipeline>   _chVkPipeline;
        VkPipelineLayout                _chVkPipelineLayout = VK_NULL_HANDLE;
    };
}