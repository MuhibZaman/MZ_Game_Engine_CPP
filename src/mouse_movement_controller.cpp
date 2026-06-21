#include "mouse_movement_controller.hpp"

namespace lve {
    void MouseMovementController::rotateCamera(GLFWwindow *window, float dt, LveGameObject &gameObject) {
        glm::vec3 rotate{0.0f};

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        double cursordx = xpos - lastCursorx;
        double cursordy = ypos - lastCursory;
        lastCursorx = xpos;
        lastCursory = ypos;
        if(cursordx > 1.75) rotate.y += 1.0f;
        if(cursordx < -1.75) rotate.y -= 1.0f;
        if(cursordy < -1.75) rotate.x += 1.0f;
        if(cursordy > 1.75) rotate.x -= 1.0f;

        if(glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
            gameObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);
        }

        //Limiting ranged of object rotate values
        gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f); //Keeps objects from going upside down
        gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>()); //Prevents value from overflowing
    }
} //namespace lve