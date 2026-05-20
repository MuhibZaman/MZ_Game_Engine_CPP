//Class that Controls Application
#pragma once

#include "lve_window.hpp"

namespace lve {
    class FirstApp {
        private:
            //lve window variable
            LveWindow lveWindow{WIDTH, HEIGHT, "Hello Vulkan!"}; //No pointer or dynamic memory allocaiton
        
        public:
            //Const width and height variables
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

            void run();
    };
} //namespace lve