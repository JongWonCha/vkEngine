#pragma once

#include <string>
#include <vector>
#include "lveDevice.hpp"

namespace lve
{
    struct PipelineConfigInfo
    {
        
    };
    class LvePipeline
    {
    public:
        LvePipeline(LveDevice& device, const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo);
        ~LvePipeline() {}

        LvePipeline(const LvePipeline&) = delete;
        LvePipeline& operator=(const LvePipeline&) = delete;

        static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

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
