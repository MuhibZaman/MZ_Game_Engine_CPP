#include "first_app.hpp"

namespace lve {
    void FirstApp::run() {
        while(!lveWindow.shouldClose()) {
            //Checks for events done on the window
            glfwPollEvents();
        }
    }
} //namespace lve