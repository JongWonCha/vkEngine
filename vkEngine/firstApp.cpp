#include "firstApp.hpp"
#include "SimpleRenderSystem.hpp"

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
    }

    FirstApp::~FirstApp()
    {
        
    }

    void FirstApp::Run()
    {
		SimpleRenderSystem simpleRenderSystem(_chVkDevice, _chVkRenderer.GetSwapChainRenderPass());
        while ( !_chVkWindow.ShouldClose() )
        {
            glfwPollEvents();

			if (VkCommandBuffer commandBuffer = _chVkRenderer.BeginFrame())
			{
                _chVkRenderer.BeginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.RenderGameObjects(commandBuffer, _chVkGameObjects);
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
}
