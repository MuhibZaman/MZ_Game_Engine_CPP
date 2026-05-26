//Class that Controls Application
#pragma once

#include "lve_window.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"
#include "lve_model.hpp"

//std
#include <memory>
#include <vector>

namespace lve {
    class FirstApp {
        private:
            //lve window variable using member initialization
            LveWindow lveWindow{WIDTH, HEIGHT, "Hello Vulkan!"}; //No pointer or dynamic memory allocaiton
            LveDevice lveDevice{lveWindow};
            LveSwapChain lveSwapChain{lveDevice, lveWindow.getExtent()};
            std::unique_ptr<LvePipeline> lvePipeline; //Automatic pointer management

            VkPipelineLayout pipelineLayout;
            std::vector<VkCommandBuffer> commandBuffers;
            std::unique_ptr<LveModel> lveModel;

            void createPipelineLayout();
            void createPipeline();
            void createCommandBuffers();

            void drawFrame();

            void loadModels();
            
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
    };
} //namespace lve