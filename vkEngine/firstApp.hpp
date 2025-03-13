#pragma once

#include "lvewindow.hpp"
#include "lvePipeline.hpp"
#include "lveDevice.hpp"
#include "lveSwapChain.hpp"
#include "lveModel.hpp"

// std
#include <memory>
#include <vector>

namespace lve
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
        void LoadModels();
        void CreatePipelineLayout();
        void CreatePipeline();
        void CreateCommandBuffers();
        void DrawFrame();
        
        LveWindow       _lveWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
        LveDevice       _lveDevice{_lveWindow};
        LveSwapChain    _lveSwapChain{_lveDevice, _lveWindow.GetExtent()};
        std::unique_ptr<LvePipeline> _lvePipeline;
        VkPipelineLayout _lvePipelineLayout = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> _commandBuffers;
        std::unique_ptr<LveModel> _lveModel;
        //LvePipeline     _lvePipeline{_lveDevice, "shaders\\simpleShader.vert.spv", "shaders\\simpleShader.frag.spv", LvePipeline::DefaultPipelineConfigInfo(WIDTH, HEIGHT)};
    };
}