#pragma once

#include "lve_device.hpp"

//std
#include <string>
#include <vector>

namespace lve {
    //Contains data specifying how we want to configure our pipeline
    struct PipelineConfigInfo {
        PipelineConfigInfo() = default;
        PipelineConfigInfo(const PipelineConfigInfo &) = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo &) = delete;
        
        std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    }; //Allows applciation to configure pipeline completely and share configs
    
    class LvePipeline {
       public:
            LvePipeline(
                LveDevice &device,
                const std::string &vertFilepath,
                const std::string &fragFilepath,
                const PipelineConfigInfo &configInfo
            );

            ~LvePipeline();

            LvePipeline(const LvePipeline &) = delete;
            LvePipeline &operator = (const LvePipeline &) = delete;

            static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);
            static void enableAlphaBlending(PipelineConfigInfo &configInfo);

            void bind(VkCommandBuffer commandBuffer);

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
    };
} //namespace lve