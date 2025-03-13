#pragma once

#include <string>
#include <vector>
#include "lveDevice.hpp"

namespace lve
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
    class LvePipeline
    {
    public:
        LvePipeline(LveDevice& device, const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo);
        ~LvePipeline();

        LvePipeline(const LvePipeline&) = delete;
        LvePipeline& operator=(const LvePipeline&) = delete;
        LvePipeline() = default;
        
        void Bind(VkCommandBuffer commandBuffer);

        static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

    private:
        static std::vector<char> ReadFile(const std::string& filePath);

        void CreateGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo);

        void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

        LveDevice&      _lveDevice;
        VkPipeline      _graphicsPipeline;
        VkShaderModule  _vertShaderModule;
        VkShaderModule  _fragShaderModule;
    };
}
