//Class that Controls Application
#pragma once

#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_game_object.hpp"

//std
#include <memory>
#include <vector>

namespace lve {
    class SimpleRenderSystem {
        public:
            SimpleRenderSystem(LveDevice &device, VkRenderPass renderPass);
            ~SimpleRenderSystem();

            SimpleRenderSystem(const SimpleRenderSystem &) = delete;
            SimpleRenderSystem &operator = (const SimpleRenderSystem &) = delete;

            void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<LveGameObject> &gameObjects);
            
        private:
            //lve window variable using member initialization
            LveDevice &lveDevice;            
            std::unique_ptr<LvePipeline> lvePipeline; //Automatic pointer management
            VkPipelineLayout pipelineLayout;

            void createPipelineLayout();
            void createPipeline(VkRenderPass renderPass);
            
    };
} //namespace lve