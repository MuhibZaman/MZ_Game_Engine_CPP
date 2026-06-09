#pragma once

#include "lve_device.hpp"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//std
#include <vector>

namespace lve {
    //Takes vertex data created by or read in a file on the CPU, and then allocate memory and copy data over to device GPU
    class LveModel {
        public:
            struct Vertex {
                //Interleaved position and color
                glm::vec3 position;
                glm::vec3 color;

                static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
                static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
            };

            struct Builder {
                //Temporary helper struct holding vertex and index data until it can be loaded into models vertex and index buffer memory
                std::vector<Vertex> vertices{};
                std::vector<uint32_t> indices{};
            };

            LveModel(LveDevice &device, const LveModel::Builder &builder);
            ~LveModel();

            LveModel(const LveModel &) = delete;
            LveModel &operator = (const LveModel &) = delete;

            void bind(VkCommandBuffer commandBuffer);
            void draw(VkCommandBuffer commandBuffer);

        private:
            LveDevice &lveDevice;

            VkBuffer vertexBuffer;
            VkDeviceMemory vertexBufferMemory;
            uint32_t vertexCount;
            void createVertexBuffers(const std::vector<Vertex> &vertices);

            bool hasIndexBuffer = false; //Can load models with and without index buffer
            VkBuffer indexBuffer;
            VkDeviceMemory indexBufferMemory;
            uint32_t indexCount;
            void createIndexBuffers(const std::vector<uint32_t> &indices);
    };
} //namespace lve