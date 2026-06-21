//Class that Controls Application
#pragma once

#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_game_object.hpp"
#include "lve_camera.hpp"
#include "lve_frame_info.hpp"

//std
#include <memory>
#include <vector>

namespace lve {
    class SimpleRenderSystem {
        public:
            SimpleRenderSystem(LveDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
            ~SimpleRenderSystem();

            SimpleRenderSystem(const SimpleRenderSystem &) = delete;
            SimpleRenderSystem &operator = (const SimpleRenderSystem &) = delete;

            void renderGameObjects(FrameInfo &frameInfo); //Want to share camera variable amongst multiple render systems
            
        private:
            //lve window variable using member initialization
            LveDevice &lveDevice;            
            std::unique_ptr<LvePipeline> lvePipeline; //Automatic pointer management
            VkPipelineLayout pipelineLayout;

            void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
            void createPipeline(VkRenderPass renderPass);            
    };
} //namespace lve