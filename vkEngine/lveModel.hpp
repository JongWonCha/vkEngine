#pragma once
#include "lveDevice.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vector>

namespace lve
{
    class LveModel
    {
    public:
        struct Vertex
        {
            glm::vec2 position;
            glm::vec3 color;
            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        LveModel(LveDevice& lveDevice, const std::vector<Vertex>& vertices);
        ~LveModel();

        LveModel(const LveModel&) = delete;
        LveModel& operator=(const LveModel&) = delete;

        void Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);
    private:
        void CreateVertexBuffers(const std::vector<Vertex>& vertices);
        
        LveDevice&      _device;
        VkBuffer        _vertexBuffer;
        VkDeviceMemory  _vertexBufferMemory;
        uint32_t        _vertexCount;
    };
}

