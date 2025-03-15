#pragma once

#include "chVkWindow.hpp"
#include "chVkPipeline.hpp"
#include "chVkDevice.hpp"
#include "chVkSwapChain.hpp"
#include "chVkGameObject.hpp"

// std
#include <memory>
#include <vector>

namespace chVk
{
    class FirstApp
    {
    public:
        static constexpr  int WIDTH = 800;
        static constexpr  int HEIGHT = 600;

        FirstApp();
        ~FirstApp();

        FirstApp(const FirstApp&) = delete;
        FirstApp &operator=(const FirstApp&) = delete;

        void Run();
    private:
        void LoadGameObjects();
        void CreatePipelineLayout();
        void CreatePipeline();
        void CreateCommandBuffers();
        void FreeCommandBuffers();
        void DrawFrame();
        void RecreateSwapChain();
        void RecordCommandBuffer(int imageIndex);
        void RenderGameObjects(VkCommandBuffer commandBuffer);
        
        chVkWindow                      _chVkWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
        chVkDevice                      _chVkDevice{_chVkWindow};
        std::unique_ptr<chVkSwapChain>  _chVkSwapChain;
        std::unique_ptr<chVkPipeline>   _chVkPipeline;
        VkPipelineLayout                _chVkPipelineLayout = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer>    _commandBuffers;
        std::vector<chVkGameObject>     _chVkGameObjects;
        //chVkPipeline     _chVkPipeline{_chVkDevice, "shaders\\simpleShader.vert.spv", "shaders\\simpleShader.frag.spv", chVkPipeline::DefaultPipelineConfigInfo(WIDTH, HEIGHT)};
    };
}