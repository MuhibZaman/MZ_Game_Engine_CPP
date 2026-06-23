#pragma once

#include "lve_device.hpp"
#include <vulkan/vulkan.h>

namespace lve {

class LveShadowMap {
    public:
        static constexpr uint32_t SHADOW_MAP_WIDTH = 2048;
        static constexpr uint32_t SHADOW_MAP_HEIGHT = 2048;
        static constexpr VkFormat SHADOW_MAP_FORMAT = VK_FORMAT_D32_SFLOAT;

        LveShadowMap(LveDevice &device);
        ~LveShadowMap();

        LveShadowMap(const LveShadowMap &) = delete;
        LveShadowMap &operator=(const LveShadowMap &) = delete;

        VkRenderPass getRenderPass() const { return shadowRenderPass; }
        VkFramebuffer getFramebuffer() const { return shadowFramebuffer; }
        VkImageView getImageView() const { return shadowImageView; }
        VkSampler getSampler() const { return shadowSampler; }
        VkDescriptorImageInfo getDescriptorImageInfo() const;

    private:
        void createShadowImage();
        void createShadowImageView();
        void createShadowSampler();
        void createShadowRenderPass();
        void createShadowFramebuffer();

        LveDevice &lveDevice;

        VkImage shadowImage;
        VkDeviceMemory shadowImageMemory;
        VkImageView shadowImageView;
        VkSampler shadowSampler;
        VkRenderPass shadowRenderPass;
        VkFramebuffer shadowFramebuffer;
    };
} // namespace lve