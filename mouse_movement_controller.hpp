#include "lve_game_object.hpp"
#include "lve_window.hpp"

namespace lve {
    class MouseMovementController {
        public:
            float lookSpeed{2.0f};
            double lastCursorx{0.0f}, lastCursory{0.0f};
            
            void rotateCamera(GLFWwindow *window, float dt, LveGameObject &gameObject); //Input dependant on GLFW windowing system
    };
} //namespace lve