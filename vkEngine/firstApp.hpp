#pragma once

#include "chVkWindow.hpp"
#include "chVkPipeline.hpp"
#include "chVkDevice.hpp"
#include "chVkGameObject.hpp"
#include "chVkRenderer.hpp"

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
        void RenderGameObjects(VkCommandBuffer commandBuffer);
        
        ChVkWindow                      _chVkWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
        ChVkDevice                      _chVkDevice{_chVkWindow};
		ChVkRenderer					_chVkRenderer{ _chVkWindow, _chVkDevice };

        std::unique_ptr<chVkPipeline>   _chVkPipeline;
        VkPipelineLayout                _chVkPipelineLayout = VK_NULL_HANDLE;
        std::vector<chVkGameObject>     _chVkGameObjects;
        //chVkPipeline     _chVkPipeline{_chVkDevice, "shaders\\simpleShader.vert.spv", "shaders\\simpleShader.frag.spv", chVkPipeline::DefaultPipelineConfigInfo(WIDTH, HEIGHT)};
    };
}