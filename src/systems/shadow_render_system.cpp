#include "shadow_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <stdexcept>
#include <array>

namespace lve {
    // Same push constants as SimpleRenderSystem - we need model matrix for vertex transform
    struct ShadowPushConstantData {
        glm::mat4 modelMatrix{1.0f};
        glm::mat4 normalMatrix{1.0f}; // keep same layout as simple render system
    };

    ShadowRenderSystem::ShadowRenderSystem(
        LveDevice &device,
        LveShadowMap &shadowMap,
        VkDescriptorSetLayout globalSetLayout)
        : lveDevice{device}, lveShadowMap{shadowMap} {
        createPipelineLayout(globalSetLayout);
        createPipeline();
    }

    ShadowRenderSystem::~ShadowRenderSystem() {
        vkDeviceWaitIdle(lveDevice.device());
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
    }

    void ShadowRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(ShadowPushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if(vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shadow pipeline layout");
        }
    }

    void ShadowRenderSystem::createPipeline() {
        assert(pipelineLayout != nullptr && "cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig);

        // Shadow pass has no color attachments
        pipelineConfig.colorBlendInfo.attachmentCount = 0;
        pipelineConfig.colorBlendInfo.pAttachments = nullptr;

        // Use the shadow render pass instead of the swap chain render pass
        pipelineConfig.renderPass = lveShadowMap.getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;

        lvePipeline = std::make_unique<LvePipeline>(
            lveDevice,
            "shaders/shadow.vert.spv",
            "shaders/shadow.frag.spv",
            pipelineConfig
        );
    }

    void ShadowRenderSystem::update(FrameInfo &frameInfo, GlobalUbo &ubo) {
        glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, 0.25f, 0.0f));
        glm::vec3 lightPos = -lightDir * 10.0f;

        glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 20.0f);
        glm::mat4 lightView = glm::lookAt(
            lightPos,
            glm::vec3(0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        ubo.lightSpaceMatrix = lightProjection * lightView;
        ubo.directionalLightDir = glm::vec4(lightDir, 0.0f);
    }

    void ShadowRenderSystem::render(FrameInfo &frameInfo) {
        // Begin shadow render pass
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = lveShadowMap.getRenderPass();
        renderPassInfo.framebuffer = lveShadowMap.getFramebuffer();
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = {
            LveShadowMap::SHADOW_MAP_WIDTH,
            LveShadowMap::SHADOW_MAP_HEIGHT
        };

        // Only need to clear depth, no color attachment
        VkClearValue clearValue{};
        clearValue.depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearValue;

        vkCmdBeginRenderPass(frameInfo.commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Set viewport and scissor to shadow map dimensions
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(LveShadowMap::SHADOW_MAP_WIDTH);
        viewport.height = static_cast<float>(LveShadowMap::SHADOW_MAP_HEIGHT);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(frameInfo.commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = {LveShadowMap::SHADOW_MAP_WIDTH, LveShadowMap::SHADOW_MAP_HEIGHT};
        vkCmdSetScissor(frameInfo.commandBuffer, 0, 1, &scissor);

        lvePipeline->bind(frameInfo.commandBuffer);

        // Bind global descriptor set so shader can access lightSpaceMatrix from UBO
        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr
        );

        // Render all game objects with models into the shadow map
        for(auto &kv : frameInfo.gameObjects) {
            auto &obj = kv.second;
            if(obj.model == nullptr) continue;

            ShadowPushConstantData push{};
            push.modelMatrix = obj.transform.mat4();
            push.normalMatrix = obj.transform.normalMatrix();

            vkCmdPushConstants(
                frameInfo.commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT,
                0,
                sizeof(ShadowPushConstantData),
                &push
            );

            obj.model->bind(frameInfo.commandBuffer);
            obj.model->draw(frameInfo.commandBuffer);
        }

        vkCmdEndRenderPass(frameInfo.commandBuffer);
    }
} // namespace lve