#include "firstApp.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <cassert>

namespace chVk
{
    struct SimplePushConstantData
    {
        glm::mat2 transform {1.f};
        // same as vec2, vec2(padding), vec3
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };


    FirstApp::FirstApp()
    {
        LoadGameObjects();
        CreatePipelineLayout();
        RecreateSwapChain();
        CreateCommandBuffers();
    }

    FirstApp::~FirstApp()
    {
        vkDestroyPipelineLayout(_chVkDevice.device(), _chVkPipelineLayout, nullptr);
    }

    void FirstApp::Run()
    {
        while ( !_chVkWindow.ShouldClose() )
        {
            glfwPollEvents();
            DrawFrame();
        }
        vkDeviceWaitIdle(_chVkDevice.device());
    }

    void FirstApp::LoadGameObjects()
    {
        std::vector<chVkModel::Vertex> vertices(3);
        vertices[0].position = { 0.0f, -0.5f };
        vertices[0].color = { 1.0f, 0.0f, 0.0f };
        vertices[1].position = { 0.5f, 0.5f };
        vertices[1].color = { 0.0f, 1.0f, 0.0f };
        vertices[2].position = { -0.5f, 0.5f };
        vertices[2].color = { 0.0f, 0.0f, 1.0f };
        

        auto _chVkModel = std::make_shared<chVkModel>(_chVkDevice, vertices);

        auto triangle = chVkGameObject::CreateGameObject();
        triangle._model = _chVkModel;
        triangle._color = glm::vec3(0.1f, 0.8f, 0.1f);
        triangle._transform2d.translation.x = 0.2f;
        triangle._transform2d.scale = { 2.f, 0.5f };
        triangle._transform2d.rotation = 0.25f * glm::two_pi<float>();
        
        _chVkGameObjects.push_back(std::move(triangle));
    }

    void FirstApp::CreatePipelineLayout()
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);



        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if ( vkCreatePipelineLayout(_chVkDevice.device(), &pipelineLayoutInfo, nullptr, &_chVkPipelineLayout) != VK_SUCCESS )
        {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void FirstApp::CreatePipeline()
    {
        PipelineConfigInfo pipelineConfig{};
        chVkPipeline::DefaultPipelineConfigInfo(pipelineConfig);

        pipelineConfig.renderPass = _chVkSwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = _chVkPipelineLayout;
        _chVkPipeline = std::make_unique<chVkPipeline>(
            _chVkDevice,
            "shaders\\simpleShader.vert.spv",
            "shaders\\simpleShader.frag.spv",
            pipelineConfig
            );
    }

    void FirstApp::CreateCommandBuffers()
    {
        assert(_chVkSwapChain != nullptr && "Cannot Create Pipeline befor SwapChain");
        assert(_chVkPipelineLayout != nullptr && "Cannot Create Pipeline befor pipeline layout");

        _commandBuffers.resize(_chVkSwapChain->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = _chVkDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());

        if ( vkAllocateCommandBuffers(_chVkDevice.device(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS )
        {
            throw std::runtime_error("failed to allocate command buffers");
        }
    }

    void FirstApp::FreeCommandBuffers()
    {
        vkFreeCommandBuffers(_chVkDevice.device(), _chVkDevice.getCommandPool(), static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
        _commandBuffers.clear();
    }

    void FirstApp::DrawFrame()
    {
        uint32_t imageIndex = 0;
        auto result = _chVkSwapChain->acquireNextImage(&imageIndex);

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

        result = _chVkSwapChain->submitCommandBuffers(&_commandBuffers[imageIndex], &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _chVkWindow.WasWindowResized())
        {
            _chVkWindow.ResetWindowResizedFlag();
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
        renderPassInfo.renderPass = _chVkSwapChain->getRenderPass();
        renderPassInfo.framebuffer = _chVkSwapChain->getFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = _chVkSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(_commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(_chVkSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(_chVkSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{ {0,0}, _chVkSwapChain->getSwapChainExtent() };

        vkCmdSetViewport(_commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(_commandBuffers[imageIndex], 0, 1, &scissor);

        RenderGameObjects(_commandBuffers[imageIndex]);
        
        vkCmdEndRenderPass(_commandBuffers[imageIndex]);

        if (vkEndCommandBuffer(_commandBuffers[imageIndex]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffers");
        }
    }
    
    void FirstApp::RenderGameObjects(VkCommandBuffer commandBuffer)
    {
        _chVkPipeline->Bind(commandBuffer);

        for (auto& obj : _chVkGameObjects)
        {
            obj._transform2d.rotation = glm::mod(obj._transform2d.rotation + 0.0001f, glm::two_pi<float>());
            SimplePushConstantData push{};
            push.offset = obj._transform2d.translation;
            push.color = obj._color;
            push.transform = obj._transform2d.mat2();

            vkCmdPushConstants(
                commandBuffer,
                _chVkPipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push);
            obj._model->Bind(commandBuffer);
            obj._model->Draw(commandBuffer);
        }
    }

    void FirstApp::RecreateSwapChain()
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
            _chVkSwapChain = std::make_unique<chVkSwapChain>(_chVkDevice, extent);
        }
        else
        {
            _chVkSwapChain = std::make_unique<chVkSwapChain>(_chVkDevice, extent, std::move(_chVkSwapChain));
            if (_chVkSwapChain->imageCount() != _commandBuffers.size())
            {
                FreeCommandBuffers();
                CreateCommandBuffers();
            }
        }
        
        
        CreatePipeline();
    }
   
}
