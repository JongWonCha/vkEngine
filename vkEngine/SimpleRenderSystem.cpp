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
		glm::mat2 transform{ 1.f };
		// same as vec2, vec2(padding), vec3
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	SimpleRenderSystem::SimpleRenderSystem(ChVkDevice& device, VkRenderPass renderPass) : _chVkDevice(device)
	{
		CreatePipelineLayout();
		CreatePipeline(renderPass);
	}

	SimpleRenderSystem::~SimpleRenderSystem()
	{
		vkDestroyPipelineLayout(_chVkDevice.device(), _chVkPipelineLayout, nullptr);
	}

	void SimpleRenderSystem::RenderGameObjects(VkCommandBuffer commandBuffer, std::vector<chVkGameObject>& gameObjects)
	{
		_chVkPipeline->Bind(commandBuffer);

		for (auto& obj : gameObjects)
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
				&push
			);

			obj._model->Bind(commandBuffer);
			obj._model->Draw(commandBuffer);
		}
	}

	void SimpleRenderSystem::CreatePipelineLayout()
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

		if (vkCreatePipelineLayout(_chVkDevice.device(), &pipelineLayoutInfo, nullptr, &_chVkPipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout");
		}
	}

	void SimpleRenderSystem::CreatePipeline(VkRenderPass renderPass)
	{
		assert(_chVkPipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		chVkPipeline::DefaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = _chVkPipelineLayout;
		_chVkPipeline = std::make_unique<chVkPipeline>(
			_chVkDevice,
			"shaders/simpleShader.vert.spv",
			"shaders/simpleShader.frag.spv",
			pipelineConfig);
	}
}