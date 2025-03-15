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
		CreatePipeline();

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

			if (VkCommandBuffer commandBuffer = _chVkRenderer.BeginFrame())
			{
                _chVkRenderer.BeginSwapChainRenderPass(commandBuffer);
				RenderGameObjects(commandBuffer);
				_chVkRenderer.EndSwapChainRenderPass(commandBuffer);
				_chVkRenderer.EndFrame();
			}
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
		assert(_chVkPipelineLayout != VK_NULL_HANDLE && "Cannot Create Pipeline Before Pipeline Layout");

        PipelineConfigInfo pipelineConfig{};
        chVkPipeline::DefaultPipelineConfigInfo(pipelineConfig);

		pipelineConfig.renderPass = _chVkRenderer.GetSwapChainRenderPass();
        pipelineConfig.pipelineLayout = _chVkPipelineLayout;
        _chVkPipeline = std::make_unique<chVkPipeline>(
            _chVkDevice,
            "shaders\\simpleShader.vert.spv",
            "shaders\\simpleShader.frag.spv",
            pipelineConfig
            );
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
}
