#include "lve_camera.hpp"

//std
#include <cassert>
#include <limits>

namespace lve {
    void LveCamera::setOrthographicProjection(
        float left, float right, float top, float bottom, float znear, float zfar) {
        projectionMatrix = glm::mat4{1.0f};
        projectionMatrix[0][0] = 2.f / (right - left);
        projectionMatrix[1][1] = 2.f / (bottom - top);
        projectionMatrix[2][2] = 1.f / (zfar - znear);
        projectionMatrix[3][0] = -(right + left) / (right - left);
        projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
        projectionMatrix[3][2] = -znear / (zfar - znear);
    }
    
    void LveCamera::setPerspectiveProjection(float fovy, float aspect, float znear, float zfar) {
        assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
        const float tanHalfFovy = tan(fovy / 2.f);
        projectionMatrix = glm::mat4{0.0f};
        projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
        projectionMatrix[1][1] = 1.f / (tanHalfFovy);
        projectionMatrix[2][2] = zfar / (zfar - znear);
        projectionMatrix[2][3] = 1.f;
        projectionMatrix[3][2] = -(zfar * znear) / (zfar - znear);
    }
} //namespace lve