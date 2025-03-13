#include "firstApp.hpp"

#include <stdexcept>
#include <array>

namespace lve
{
    FirstApp::FirstApp()
    {
        LoadModels();
        CreatePipelineLayout();
        RecreateSwapChain();
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
            DrawFrame();
        }
        vkDeviceWaitIdle(_lveDevice.device());
    }

    void FirstApp::LoadModels()
    {
        std::vector<LveModel::Vertex> vertices(3);
        vertices[0].position = { 0.0f, -0.5f };
        vertices[0].color = { 1.0f, 0.0f, 0.0f };
        vertices[1].position = { 0.5f, 0.5f };
        vertices[1].color = { 0.0f, 1.0f, 0.0f };
        vertices[2].position = { -0.5f, 0.5f };
        vertices[2].color = { 0.0f, 0.0f, 1.0f };
        

        _lveModel = std::make_unique<LveModel>(_lveDevice, vertices);
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
        PipelineConfigInfo pipelineConfig{};
        LvePipeline::DefaultPipelineConfigInfo(pipelineConfig);

        pipelineConfig.renderPass = _lveSwapChain->getRenderPass();
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
        assert(_lveSwapChain != nullptr && "Cannot Create Pipeline befor SwapChain");
        assert(_lvePipelineLayout != nullptr && "Cannot Create Pipeline befor pipeline layout");

        _commandBuffers.resize(_lveSwapChain->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = _lveDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());

        if ( vkAllocateCommandBuffers(_lveDevice.device(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS )
        {
            throw std::runtime_error("failed to allocate command buffers");
        }
    }

    void FirstApp::FreeCommandBuffers()
    {
        vkFreeCommandBuffers(_lveDevice.device(), _lveDevice.getCommandPool(), static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
        _commandBuffers.clear();
    }

    void FirstApp::DrawFrame()
    {
        uint32_t imageIndex = 0;
        auto result = _lveSwapChain->acquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapChain();
            return;
        }

        if ( result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR )
        {
            throw std::runtime_error("failed to acquire swap chain image");
        }

        RecordCommandBuffer(imageIndex);

        result = _lveSwapChain->submitCommandBuffers(&_commandBuffers[imageIndex], &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _lveWindow.WasWindowResized())
        {
            _lveWindow.ResetWindowResizedFlag();
            RecreateSwapChain();
            return;
        }

        if ( result != VK_SUCCESS )
        {
            throw std::runtime_error("failed to submit swap chain command buffers");
        }
    }

    void FirstApp::RecordCommandBuffer(int imageIndex)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        if (vkBeginCommandBuffer(_commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffers");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = _lveSwapChain->getRenderPass();
        renderPassInfo.framebuffer = _lveSwapChain->getFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = _lveSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(_commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(_lveSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(_lveSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{ {0,0}, _lveSwapChain->getSwapChainExtent() };

        vkCmdSetViewport(_commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(_commandBuffers[imageIndex], 0, 1, &scissor);

        _lvePipeline->Bind(_commandBuffers[imageIndex]);
        _lveModel->Bind(_commandBuffers[imageIndex]);
        _lveModel->Draw(_commandBuffers[imageIndex]);

        vkCmdEndRenderPass(_commandBuffers[imageIndex]);

        if (vkEndCommandBuffer(_commandBuffers[imageIndex]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffers");
        }
    }

    void FirstApp::RecreateSwapChain()
    {
        auto extent = _lveWindow.GetExtent();
        while (extent.width == 0 || extent.height == 0)
        {
            extent = _lveWindow.GetExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(_lveDevice.device());

        if (_lveSwapChain == nullptr)
        {
            _lveSwapChain = std::make_unique<LveSwapChain>(_lveDevice, extent);
        }
        else
        {
            _lveSwapChain = std::make_unique<LveSwapChain>(_lveDevice, extent, std::move(_lveSwapChain));
            if (_lveSwapChain->imageCount() != _commandBuffers.size())
            {
                FreeCommandBuffers();
                CreateCommandBuffers();
            }
        }
        
        
        CreatePipeline();
    }
   
}
