#pragma once
#include "chVkDevice.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vector>

namespace chVk
{
    class ChVkModel
    {
    public:
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 color;
            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        ChVkModel(ChVkDevice& chVkDevice, const std::vector<Vertex>& vertices);
        ~ChVkModel();

        ChVkModel(const ChVkModel&) = delete;
        ChVkModel& operator=(const ChVkModel&) = delete;

        void Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);
    private:
        void CreateVertexBuffers(const std::vector<Vertex>& vertices);
        
        ChVkDevice&      _device;
        VkBuffer        _vertexBuffer;
        VkDeviceMemory  _vertexBufferMemory;
        uint32_t        _vertexCount;
    };
}

