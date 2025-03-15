#pragma once

#include <string>
#include <vector>
#include "chVkDevice.hpp"

namespace chVk
{
    struct PipelineConfigInfo
    {
        /*VkViewport                                  viewport{};
        VkRect2D                                    scissor{};*/
        VkPipelineViewportStateCreateInfo           viewportInfo{};
        VkPipelineInputAssemblyStateCreateInfo      inputAssemblyInfo {};
        VkPipelineRasterizationStateCreateInfo      rasterizationInfo{};
        VkPipelineMultisampleStateCreateInfo        multisampleInfo{};
        VkPipelineColorBlendAttachmentState         colorBlendAttachment{};
        VkPipelineColorBlendStateCreateInfo         colorBlendInfo{};
        VkPipelineDepthStencilStateCreateInfo       depthStencilInfo{};
        std::vector<VkDynamicState>                 dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo            dynamicStateInfo{};
        VkPipelineLayout                            pipelineLayout = nullptr;
        VkRenderPass                                renderPass = nullptr;
        uint32_t                                    subpass = 0;
    };
    class chVkPipeline
    {
    public:
        chVkPipeline(ChVkDevice& device, const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo);
        ~chVkPipeline();

        chVkPipeline(const chVkPipeline&) = delete;
        chVkPipeline& operator=(const chVkPipeline&) = delete;
        chVkPipeline() = default;
        
        void Bind(VkCommandBuffer commandBuffer);

        static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

    private:
        static std::vector<char> ReadFile(const std::string& filePath);

        void CreateGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo);

        void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

        ChVkDevice&      _chVkDevice;
        VkPipeline      _graphicsPipeline;
        VkShaderModule  _vertShaderModule;
        VkShaderModule  _fragShaderModule;
    };
}
