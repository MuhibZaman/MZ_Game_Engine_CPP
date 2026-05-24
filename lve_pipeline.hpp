#pragma once

#include "lve_device.hpp"

//std
#include <string>
#include <vector>

namespace lve {
    //Contains data specifying how we want to configure our pipeline
    struct PipelineConfigInfo {
        PipelineConfigInfo(const PipelineConfigInfo &) = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo &) = delete;

        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    }; //Allows applciation to configure pipeline completely and share configs
    
    class LvePipeline {
        private:
            static std::vector<char> readFile(const std::string &filepath);

            void createGraphicsPipeline(
                const std::string &vertFilepath,
                const std::string &fragFilepath,
                const PipelineConfigInfo &configInfo
            );

            void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

            //Implicit relationship that member relationship WILL outlive any instance of the containing class; Aggrigation
            LveDevice& lveDevice;
            VkPipeline graphicsPipeline;
            VkShaderModule vertShaderModule;
            VkShaderModule fragShaderModule;

        public:
            LvePipeline(
                LveDevice &device,
                const std::string &vertFilepath,
                const std::string &fragFilepath,
                const PipelineConfigInfo &configInfo
            );

            ~LvePipeline();

            LvePipeline(const LvePipeline &) = delete;
            void operator = (const LvePipeline &) = delete;

            static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo, uint32_t width, uint32_t height);

            void bind(VkCommandBuffer commandBuffer);
    };
} //namespace lve