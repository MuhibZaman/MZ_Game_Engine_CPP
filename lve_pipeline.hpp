#pragma once

#include "lve_device.hpp"

//std
#include <string>
#include <vector>

namespace lve {
    //Contains data specifying how we want to configure our pipeline
    struct PipelineConfigInfo {}; //Allows applciation to configure pipeline completely and share configs

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

            ~LvePipeline() {}

            LvePipeline(const LvePipeline &) = delete;
            void operator = (const LvePipeline &) = delete;

            static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);
    };
} //namespace lve