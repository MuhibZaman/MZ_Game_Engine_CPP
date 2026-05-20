//Window Method Implementations
#include "lve_window.hpp"

namespace lve {
    //Member initializer list
    LveWindow::LveWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name} {
        initWindow();
    }

    LveWindow::~LveWindow() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void LveWindow::initWindow() {
        glfwInit();
        //Hint to not create OpenGL context since we are using Vulkan
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        //Hint to disable our window from being resized after creation. We handle window resize ourselves
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        //Initializing window pointer by creating glfw window
        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr); //$4 = not full screen
    }
} //namespace lve