#include "lveModel.hpp"
#include <cassert>
#include <cstring>

namespace lve
{
    LveModel::LveModel(LveDevice& lveDevice, const std::vector<Vertex>& vertices) : _device(lveDevice)
    {
        CreateVertexBuffers(vertices);
        
    }

    LveModel::~LveModel()
    {
        vkDestroyBuffer(_device.device(), _vertexBuffer, nullptr);
        vkFreeMemory(_device.device(), _vertexBufferMemory, nullptr);
    }

    void LveModel::CreateVertexBuffers(const std::vector<Vertex>& vertices)
    {
        _vertexCount = static_cast<uint32_t>(vertices.size());
        assert(_vertexCount >= 3 && "Vertex count must be greater than 3 or equal to 3");
        VkDeviceSize bufferSize = sizeof(Vertex) * _vertexCount;


        // host = CPU, Device = GPU
        _device.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            _vertexBuffer,
            _vertexBufferMemory);

        void* data = nullptr;
        vkMapMemory(_device.device(), _vertexBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<uint32_t>(bufferSize));
        vkUnmapMemory(_device.device(), _vertexBufferMemory);
    }

    void LveModel::Bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[] = { _vertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }

    void LveModel::Draw(VkCommandBuffer commandBuffer)
    {
        vkCmdDraw(commandBuffer, _vertexCount, 1, 0, 0);
    }

    std::vector<VkVertexInputBindingDescription> LveModel::Vertex::getBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> LveModel::Vertex::getAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = 0;
        return attributeDescriptions;
    }
}