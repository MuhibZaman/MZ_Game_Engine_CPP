#pragma once

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace lve {
    class LveCamera {
        public:
            void setOrthographicProjection(float left, float right, float top, float bottom, float znear, float zfar);
            void setPerspectiveProjection(float fovy, float aspect, float znear, float zfar);
            const glm::mat4 &getProjection() const { return projectionMatrix; };

        private:
            glm::mat4 projectionMatrix{1.0f};
    };
} //namespace lve