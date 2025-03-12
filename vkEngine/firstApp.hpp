#pragma once

#include "lvewindow.hpp"
#include "lvePipeline.hpp"
#include "lveDevice.hpp"

namespace lve
{
    class FirstApp
    {
    public:
        static constexpr  int WIDTH = 800;
        static constexpr  int HEIGHT = 600;

        void Run();
    private:
        LveWindow   _lveWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
        LveDevice   _lveDevice{_lveWindow};
        LvePipeline _lvePipeline{_lveDevice, "shaders\\simpleShader.vert.spv", "shaders\\simpleShader.frag.spv", LvePipeline::DefaultPipelineConfigInfo(WIDTH, HEIGHT)};
    };
}