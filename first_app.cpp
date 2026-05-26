#include "first_app.hpp"

//std
#include <stdexcept>
#include <array>
#include <iostream>

namespace lve {
    FirstApp::FirstApp() {
        loadModels();
        createPipelineLayout();
        createPipeline();
        createCommandBuffers();
    }

    FirstApp::~FirstApp() {
        vkDeviceWaitIdle(lveDevice.device());
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
    }

    void FirstApp::run() {
        while(!lveWindow.shouldClose()) {
            //Checks for events done on the window
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(lveDevice.device());
    }

    void FirstApp::fractal(std::vector<LveModel::Vertex> *vertices, LveModel::Vertex one, LveModel::Vertex two, LveModel::Vertex three, int depth) {
        if(depth == 5) {
            vertices->push_back(one);
            vertices->push_back(two);
            vertices->push_back(three);
            return;
        }
        
        float newOne_x = (one.position[0] + two.position[0]) / 2.0f;
        float newOne_y = (one.position[1] + two.position[1]) / 2.0f;
        LveModel::Vertex midOne = {{newOne_x, newOne_y}};

        float newTwo_x = (two.position[0] + three.position[0]) / 2.0f;
        float newTwo_y = (two.position[1] + three.position[1]) / 2.0f;
        LveModel::Vertex midTwo = {{newTwo_x, newTwo_y}};

        float newThree_x = (three.position[0] + one.position[0]) / 2.0f;
        float newThree_y = (three.position[1] + one.position[1]) / 2.0f;
        LveModel::Vertex midThree = {{newThree_x, newThree_y}};

        fractal(vertices, one, midOne, midThree, depth + 1);
        fractal(vertices, midOne, two, midTwo, depth + 1);
        fractal(vertices, midThree, midTwo, three, depth + 1);
    }

    void FirstApp::loadModels() {
        std::vector<LveModel::Vertex> vertices;
        fractal(&vertices,
            {{0.0f, -0.5f}},
            {{0.5f, 0.5f}},
            {{-0.5f, 0.5f}},
            0
        );
        lveModel = std::make_unique<LveModel>(lveDevice, vertices);
    }

    void FirstApp::createPipelineLayout() {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr; //Used to pass data other than vertex data to shaders, ie textures
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr; //Used to send small amounts of data directly to shaders
        
        if(vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void FirstApp::createPipeline() {
        PipelineConfigInfo pipelineConfig{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig, lveSwapChain.width(), lveSwapChain.height());
        pipelineConfig.renderPass = lveSwapChain.getRenderPass(); //Render pass descries structure and format of framebuffer objects and attachments
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<LvePipeline>(
            lveDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig
        ); //Path relative to exe directory
    }

    void FirstApp::createCommandBuffers() {
        commandBuffers.resize(lveSwapChain.imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = lveDevice.getCommandPool(); //Space set asside for command buffer creation
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if(vkAllocateCommandBuffers(lveDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        for (int i = 0; i < commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if(vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = lveSwapChain.getRenderPass();
            renderPassInfo.framebuffer = lveSwapChain.getFrameBuffer(i);

            //Render area, where shader loads and stores takes place
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = lveSwapChain.getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.1f, 0.1f, 0.1f, 0.1f}; //Framebuffer[0] = color
            clearValues[1].depthStencil = {1.0f, 0}; //Framebuffer[1] = depth
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            lvePipeline->bind(commandBuffers[i]);
            lveModel->bind(commandBuffers[i]);
            lveModel->draw(commandBuffers[i]);

            vkCmdEndRenderPass(commandBuffers[i]);
            if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to recond command buffers!");
            }
        }
    }
            
    void FirstApp::drawFrame() {
        uint32_t imageIndex;
        auto result = lveSwapChain.acquireNextImage(&imageIndex); //Index of frame we should render to next , and hangles CPU-GPU synchronizations

        if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image");
        }

        result = lveSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex); //Submits provided command buffer to device graphics queue, while handing synconization. Once command buffer is executed, swap chain will present associated color attachment image view to the display at the appropriate time based on present mode
        if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to present swap chain image");
        }
    }
} //namespace lve