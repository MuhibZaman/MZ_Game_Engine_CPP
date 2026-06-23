//Renders into shadow map and not the swap chain
#pragma once

#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_game_object.hpp"
#include "lve_frame_info.hpp"
#include "lve_shadow_map.hpp"

#include <memory>
#include <vector>

namespace lve {
    class ShadowRenderSystem {
        public:
            ShadowRenderSystem(LveDevice &device, LveShadowMap &shadowMap, VkDescriptorSetLayout globalSetLayout);
            ~ShadowRenderSystem();

            ShadowRenderSystem(const ShadowRenderSystem &) = delete;
            ShadowRenderSystem &operator=(const ShadowRenderSystem &) = delete;

            void update(FrameInfo &frameInfo, GlobalUbo &ubo);
            void render(FrameInfo &frameInfo);            

        private:
            LveDevice &lveDevice;
            LveShadowMap &lveShadowMap;
            std::unique_ptr<LvePipeline> lvePipeline;
            VkPipelineLayout pipelineLayout;

            void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
            void createPipeline();
    };
} // namespace lve