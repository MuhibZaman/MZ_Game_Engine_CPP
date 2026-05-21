//Class that Controls Application
#pragma once

#include "lve_window.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"

namespace lve {
    class FirstApp {
        private:
            //lve window variable using member initialization
            LveWindow lveWindow{WIDTH, HEIGHT, "Hello Vulkan!"}; //No pointer or dynamic memory allocaiton
            LveDevice lveDevice{lveWindow};
            LvePipeline lvePipeline{lveDevice,
                "shaders/simple_shader.vert.spv",
                "shaders/simple_shader.frag.spv",
                LvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)
            }; //Path relative to exe
            
        public:
            //Const width and height variables
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

            //What the app does
            void run();
    };
} //namespace lve