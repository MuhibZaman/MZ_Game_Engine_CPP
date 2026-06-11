//All frame relevant info into a single object
#pragma once

#include "lve_camera.hpp"

//lib
#include <vulkan/vulkan.h>

namespace lve {
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        LveCamera &camera;
    };
} //namespace lve