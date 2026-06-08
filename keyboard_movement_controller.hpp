//Simple example of how to get keyboard inputs from user and move game objects in real time
#pragma once

#include "lve_game_object.hpp"
#include "lve_window.hpp"

namespace lve {
    class KeyboardMovementController {
        public:
            //Stores relationships between different movement actions and glfw keycodes
            struct KeyMappings {
                int moveLeft = GLFW_KEY_A;
                int moveRight = GLFW_KEY_D;
                int moveForward = GLFW_KEY_W;
                int moveBackward = GLFW_KEY_S;
                int moveUp = GLFW_KEY_SPACE;
                int moveDown = GLFW_KEY_LEFT_CONTROL;
                int lookLeft = GLFW_KEY_LEFT;
                int lookRight = GLFW_KEY_RIGHT;
                int lookUp = GLFW_KEY_UP;
                int lookDown = GLFW_KEY_DOWN;
            };
        
            KeyMappings keys{};
            float moveSpeed{3.0f};
            float lookSpeed{1.5f};
            
            void moveInPlaneXZ(GLFWwindow *window, float dt, LveGameObject &gameObject); //Input dependant on GLFW windowing system
    };
} //namespace lve