#include "lvePipeline.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

namespace lve
{
    LvePipeline::LvePipeline(LveDevice& device, const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo) : _lveDevice(device)
    {
        CreateGraphicsPipeline(vertFilePath, fragFilePath, configInfo);
    }
    
    void LvePipeline::CreateGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo)
    {
        auto vertCode = ReadFile(vertFilePath);
        auto fragCode = ReadFile(fragFilePath);
        
        std::cout << "Vertex Shader Code Size : " << vertCode.size() << "\n";
        std::cout << "Fragment Shader Code Size : " << fragCode.size() << "\n";
    }

    void LvePipeline::CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
    {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
        if ( vkCreateShaderModule(_lveDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS )
        {
            throw std::runtime_error("failed to create shader module!");
        }
    }

    PipelineConfigInfo LvePipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height)
    {
        PipelineConfigInfo configInfo = {};

        return configInfo;
    }

    std::vector<char> LvePipeline::ReadFile(const std::string& filePath)
    {
        std::ifstream file(filePath, std::ios::ate | std::ios::binary);

        if ( !file.is_open() )
        {
            throw std::runtime_error("Failed to open file" + filePath);
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();
        return buffer;
    }
}
