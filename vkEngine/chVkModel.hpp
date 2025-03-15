#pragma once
#include "chVkDevice.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vector>

namespace chVk
{
    class chVkModel
    {
    public:
        struct Vertex
        {
            glm::vec2 position;
            glm::vec3 color;
            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        chVkModel(ChVkDevice& chVkDevice, const std::vector<Vertex>& vertices);
        ~chVkModel();

        chVkModel(const chVkModel&) = delete;
        chVkModel& operator=(const chVkModel&) = delete;

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

