//Little Vulkan Engine Window
#pragma once
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <string>

namespace lve {
    //Our custom window itself
    //Stores: width, height, and name
    class LveWindow {
        private:
            void initWindow();
            static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

            //Variables window sizing
            int width;
            int height;
            bool framebufferResized = false;
            //Variable holding window name
            std::string windowName;
            
            //Pointer to GFLFW window
            //GLFW talks to OS and window manager
            //Handles window creation, input, and event loop
            GLFWwindow *window;

        public:
            //Constructor
            LveWindow(int w, int h, std::string name);
            //Destructor
            ~LveWindow();            

            //Deleting copy constructor and copy operator from window class
            //Since we are using a pointer to our glfw window
            //Resource acquisition = initializaiton
            //Resource creation happens when variables are initialized
            //Cleanups are performed by constructors
            LveWindow(const LveWindow &) = delete;
            LveWindow &operator = (const LveWindow &) = delete;

            //Inline implementation calling glfw function quering if user has tried to dismiss the window
            bool shouldClose() { return glfwWindowShouldClose(window); }
            VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
            bool wasWindowResized() { return framebufferResized; }
            void resetWindowResizedFrag() { framebufferResized = false; }

            void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
    };
} //namespace lve