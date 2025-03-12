#include "firstApp.hpp"

#include <stdexcept>
#include <array>

namespace lve
{
    FirstApp::FirstApp()
    {
        CreatePipelineLayout();
        CreatePipeline();
        CreateCommandBuffers();
    }

    FirstApp::~FirstApp()
    {
        vkDestroyPipelineLayout(_lveDevice.device(), _lvePipelineLayout, nullptr);
    }

    void FirstApp::Run()
    {
        while ( !_lveWindow.ShouldClose() )
        {
            glfwPollEvents();
            drawFrame();
        }
        vkDeviceWaitIdle(_lveDevice.device());
    }

    void FirstApp::CreatePipelineLayout()
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        if ( vkCreatePipelineLayout(_lveDevice.device(), &pipelineLayoutInfo, nullptr, &_lvePipelineLayout) != VK_SUCCESS )
        {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void FirstApp::CreatePipeline()
    {
        auto pipelineConfig = LvePipeline::DefaultPipelineConfigInfo(_lveSwapChain.width(), _lveSwapChain.height());

        pipelineConfig.renderPass = _lveSwapChain.getRenderPass();
        pipelineConfig.pipelineLayout = _lvePipelineLayout;
        _lvePipeline = std::make_unique<LvePipeline>(
            _lveDevice,
            "shaders\\simpleShader.vert.spv",
            "shaders\\simpleShader.frag.spv",
            pipelineConfig
            );
    }

    void FirstApp::CreateCommandBuffers()
    {
        _CommandBuffers.resize(_lveSwapChain.imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = _lveDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(_CommandBuffers.size());

        if ( vkAllocateCommandBuffers(_lveDevice.device(), &allocInfo, _CommandBuffers.data()) != VK_SUCCESS )
        {
            throw std::runtime_error("failed to allocate command buffers");
        }

        for (int i = 0; i < _CommandBuffers.size(); i++)
        {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            if ( vkBeginCommandBuffer(_CommandBuffers[i], &beginInfo) != VK_SUCCESS )
            {
                throw std::runtime_error("failed to begin recording command buffers");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = _lveSwapChain.getRenderPass();
            renderPassInfo.framebuffer = _lveSwapChain.getFrameBuffer(i);

            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = _lveSwapChain.getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
            clearValues[1].depthStencil = { 1.0f, 0 };
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(_CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            _lvePipeline->Bind(_CommandBuffers[i]);
            vkCmdDraw(_CommandBuffers[i], 3, 1, 0, 0);

            vkCmdEndRenderPass(_CommandBuffers[i]);

            if ( vkEndCommandBuffer(_CommandBuffers[i]) != VK_SUCCESS )
            {
                throw std::runtime_error("failed to record command buffers");
            }
        }
    }

    void FirstApp::drawFrame()
    {
        uint32_t imageIndex = 0;
        auto result = _lveSwapChain.acquireNextImage(&imageIndex);

        if ( result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR )
        {
            throw std::runtime_error("failed to acquire swap chain image");
        }
        result = _lveSwapChain.submitCommandBuffers(&_CommandBuffers[imageIndex], &imageIndex);

        if ( result != VK_SUCCESS )
        {
            throw std::runtime_error("failed to submit swap chain command buffers");
        }
    }
}
