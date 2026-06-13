//Class that Controls Application
#pragma once

#include "lve_window.hpp"
#include "lve_device.hpp"
#include "lve_game_object.hpp"
#include "lve_renderer.hpp"
#include "lve_descriptors.hpp"

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
            LveRenderer lveRenderer{lveWindow, lveDevice};

            //Order of declerations matter
            std::unique_ptr<LveDescriptorPool> globalPool{};
            std::vector<LveGameObject> gameObjects;

            void loadGameObjects();            
    };
} //namespace lve