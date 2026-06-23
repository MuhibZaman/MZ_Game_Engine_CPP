#include "lve_shadow_map.hpp"

//std
#include <stdexcept>
#include <array>

namespace lve {
    LveShadowMap::LveShadowMap(LveDevice &device) : lveDevice{device} {
        createShadowImage();
        createShadowImageView();
        createShadowSampler();
        createShadowRenderPass();
        createShadowFramebuffer();
    }

    LveShadowMap::~LveShadowMap() {
        vkDestroyFramebuffer(lveDevice.device(), shadowFramebuffer, nullptr);
        vkDestroyRenderPass(lveDevice.device(), shadowRenderPass, nullptr);
        vkDestroySampler(lveDevice.device(), shadowSampler, nullptr);
        vkDestroyImageView(lveDevice.device(), shadowImageView, nullptr);
        vkDestroyImage(lveDevice.device(), shadowImage, nullptr);
        vkFreeMemory(lveDevice.device(), shadowImageMemory, nullptr);
    }

    void LveShadowMap::createShadowImage() {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = SHADOW_MAP_WIDTH;
        imageInfo.extent.height = SHADOW_MAP_HEIGHT;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = SHADOW_MAP_FORMAT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        // Used as depth attachment AND sampled in fragment shader
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        lveDevice.createImageWithInfo(
            imageInfo,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            shadowImage,
            shadowImageMemory
        );
    }

    void LveShadowMap::createShadowImageView() {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = shadowImage;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = SHADOW_MAP_FORMAT;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if(vkCreateImageView(lveDevice.device(), &viewInfo, nullptr, &shadowImageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shadow map image view!");
        }
    }

    void LveShadowMap::createShadowSampler() {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        // Clamp to border so areas outside shadow map aren't incorrectly shadowed
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE; // depth 1.0 = not in shadow
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 1.0f;

        if(vkCreateSampler(lveDevice.device(), &samplerInfo, nullptr, &shadowSampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shadow map sampler!");
        }
    }

    void LveShadowMap::createShadowRenderPass() {
        // Only a depth attachment, no color attachment
        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = SHADOW_MAP_FORMAT;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;     // clear depth at start
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;   // keep depth after pass
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        // Final layout must be shader read since we sample it in fragment shader
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 0;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 0;       // no color output
        subpass.pColorAttachments = nullptr;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        // Two dependencies to handle the layout transition correctly
        std::array<VkSubpassDependency, 2> dependencies{};

        // Transition from shader read to depth write at start of pass
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        // Transition from depth write back to shader read at end of pass
        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &depthAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
        renderPassInfo.pDependencies = dependencies.data();

        if(vkCreateRenderPass(lveDevice.device(), &renderPassInfo, nullptr, &shadowRenderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shadow render pass!");
        }
    }

    void LveShadowMap::createShadowFramebuffer() {
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = shadowRenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = &shadowImageView;
        framebufferInfo.width = SHADOW_MAP_WIDTH;
        framebufferInfo.height = SHADOW_MAP_HEIGHT;
        framebufferInfo.layers = 1;

        if(vkCreateFramebuffer(lveDevice.device(), &framebufferInfo, nullptr, &shadowFramebuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shadow framebuffer!");
        }
    }

    VkDescriptorImageInfo LveShadowMap::getDescriptorImageInfo() const {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = shadowSampler;
        imageInfo.imageView = shadowImageView;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        return imageInfo;
    }
} // namespace lve