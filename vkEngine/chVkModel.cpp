﻿#include "chVkModel.hpp"
#include <cassert>
#include <cstring>

namespace chVk
{
    chVkModel::chVkModel(ChVkDevice& chVkDevice, const std::vector<Vertex>& vertices) : _device(chVkDevice)
    {
        CreateVertexBuffers(vertices);
        
    }

    chVkModel::~chVkModel()
    {
        vkDestroyBuffer(_device.device(), _vertexBuffer, nullptr);
        vkFreeMemory(_device.device(), _vertexBufferMemory, nullptr);
    }

    void chVkModel::CreateVertexBuffers(const std::vector<Vertex>& vertices)
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

    void chVkModel::Bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[] = { _vertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }

    void chVkModel::Draw(VkCommandBuffer commandBuffer)
    {
        vkCmdDraw(commandBuffer, _vertexCount, 1, 0, 0);
    }

    std::vector<VkVertexInputBindingDescription> chVkModel::Vertex::getBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> chVkModel::Vertex::getAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0; // vertex Shader layout 0
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);
        return attributeDescriptions;
    }
}