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

    std::unique_ptr<ChVkModel> FirstApp::CreateCubeModel(ChVkDevice& device, glm::vec3 offset)
    {
        std::vector<ChVkModel::Vertex> vertices
        {
            // left face (white)
          {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
          {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
          {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
          {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
          {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
          {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
 
          // right face (yellow)
          {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
          {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
          {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
          {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
          {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
          {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
 
          // top face (orange, remember y axis points down)
          {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
          {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
          {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
          {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
          {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
          {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
 
          // bottom face (red)
          {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
          {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
          {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
          {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
          {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
          {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
 
          // nose face (blue)
          {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
          {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
          {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
          {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
          {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
          {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
 
          // tail face (green)
          {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
          {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
          {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
          {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
          {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
          {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        };

        for (auto& v : vertices)
        {
            v.position += offset;
        }

        return std::make_unique<ChVkModel>(device, vertices);
    }

    void FirstApp::LoadGameObjects()
    {
        std::shared_ptr<ChVkModel> model = CreateCubeModel(_chVkDevice, {0.0f, 0.0f, 0.0f});
        
        auto cube = ChVkGameObject::CreateGameObject();
        cube._model = model;
        cube._transform.translation = glm::vec3(0.0f, 0.0f, 0.5f);
        cube._transform.scale = glm::vec3(0.5f, 0.5f, 0.5f);
        _chVkGameObjects.push_back(std::move(cube));
    }
}
