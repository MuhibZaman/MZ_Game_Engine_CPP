//Class that Controls Application
#pragma once

#include "lve_window.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"
#include "lve_game_object.hpp"

//std
#include <memory>
#include <vector>

namespace lve {
    class FirstApp {
        public:
            //Const width and height variables
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

            FirstApp();
            ~FirstApp();

            FirstApp(const FirstApp &) = delete;
            FirstApp &operator = (const FirstApp &) = delete;

            //What the app does
            void run();
            
        private:
            //lve window variable using member initialization
            LveWindow lveWindow{WIDTH, HEIGHT, "Hello Vulkan!"}; //No pointer or dynamic memory allocaiton
            LveDevice lveDevice{lveWindow};
            std::unique_ptr<LveSwapChain> lveSwapChain;
            std::unique_ptr<LvePipeline> lvePipeline; //Automatic pointer management

            VkPipelineLayout pipelineLayout;
            std::vector<VkCommandBuffer> commandBuffers;
            std::vector<LveGameObject> gameObjects;

            void createPipelineLayout();
            void createPipeline();
            void createCommandBuffers();

            void drawFrame();
            void loadGameObjects();
            void renderGameObjects(VkCommandBuffer commandBuffer);

            void recreateSwapChain();
            void recordCommandBuffer(int imageIndex);
            void freeCommandBuffers();
            
    };
} //namespace lve