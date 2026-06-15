#include "first_app.hpp"
#include "systems/simple_render_system.hpp"
#include "systems/point_light_system.hpp"
#include "lve_camera.hpp"
#include "keyboard_movement_controller.hpp"
#include "mouse_movement_controller.hpp"
#include "lve_buffer.hpp"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <stdexcept>
#include <array>
#include <iostream>
#include <chrono>

namespace lve {
    FirstApp::FirstApp() {
        //Method chaining / fluent interface
        globalPool = LveDescriptorPool::Builder(lveDevice)
            .setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();

        loadGameObjects();
    }

    FirstApp::~FirstApp() {}

    void FirstApp::run() {
        std::vector<std::unique_ptr<LveBuffer>> uboBuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        //Need to make_unique over each unique_ptr in vector
        for(int i = 0; i < uboBuffers.size(); i++) {
            uboBuffers[i] = std::make_unique<LveBuffer>(
                lveDevice,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            );

            uboBuffers[i]->map();
        }

        //Create sets based on binding frequency
        auto globalSetLayout = LveDescriptorSetLayout::Builder(lveDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            LveDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

        SimpleRenderSystem simpleRenderSystem{lveDevice, lveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
        PointLightSystem pointLightSystem{lveDevice, lveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
        LveCamera camera{};
        //camera.setViewDirection(glm::vec3(0.0f), glm::vec3(0.5f, 0.0f, 1.0f));
        camera.setViewTarget(glm::vec3(-1.0f, -2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 2.5f));

        auto viewerObject = LveGameObject::createGameObject(); //No model and wont be rendered. Just used to store camera state
        viewerObject.transform.translation.z = -2.5f;
        KeyboardMovementController cameraController{};
        MouseMovementController cameraRotationController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        while(!lveWindow.shouldClose()) {
            //Checks for events done on the window
            glfwPollEvents();
            auto newTime = std::chrono::high_resolution_clock::now(); //After poll events since poll events can pause the application
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            //frameTime = glm::min(frameTime, MAX_FRAME_TIME); For not having frameTime jumping a large amount if the game gets paused inbetween key presses
            currentTime = newTime;

            cameraController.moveInPlaneXZ(lveWindow.getGLFWwindow(), frameTime, viewerObject);
            cameraRotationController.rotateCamera(lveWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = lveRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f); //fovy in radians
            if(auto commandBuffer = lveRenderer.beginFrame()) {
                int frameIndex = lveRenderer.getFrameIndex();
                //Setting up frameInfo
                FrameInfo frameInfo {
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex],
                    gameObjects
                };

                //update objects phase
                GlobalUbo ubo{};
                ubo.projection = camera.getProjection();
                ubo.view = camera.getView();
                pointLightSystem.update(frameInfo, ubo);
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                //render phase
                lveRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo);
                pointLightSystem.render(frameInfo);
                lveRenderer.endSwapChainRenderPass(commandBuffer);
                lveRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(lveDevice.device());
    }

    void FirstApp::loadGameObjects() {
        std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(lveDevice, "models/flat_vase.obj");
        auto gameObject1 = LveGameObject::createGameObject();
        gameObject1.model = lveModel;
        gameObject1.transform.translation = {-0.5f, 0.5f, 0.0f};
        gameObject1.transform.scale = glm::vec3(3.0f);
        gameObjects.emplace(gameObject1.getId(), std::move(gameObject1));

        lveModel = LveModel::createModelFromFile(lveDevice, "models/smooth_vase.obj");
        auto gameObject2 = LveGameObject::createGameObject();
        gameObject2.model = lveModel;
        gameObject2.transform.translation = {0.5f, 0.5f, 0.0f};
        gameObject2.transform.scale = glm::vec3(3.0f);
        gameObjects.emplace(gameObject2.getId(), std::move(gameObject2));

        lveModel = LveModel::createModelFromFile(lveDevice, "models/quad.obj");
        auto gameObject3 = LveGameObject::createGameObject();
        gameObject3.model = lveModel;
        gameObject3.transform.translation = {0.0f, 0.55f, 0.0f};
        gameObject3.transform.scale = glm::vec3(3.0f, 1.0f, 3.0f);
        gameObjects.emplace(gameObject3.getId(), std::move(gameObject3));

        std::vector<glm::vec3> lightColors{
            {1.f, .1f, .1f},
            {.1f, .1f, 1.f},
            {.1f, 1.f, .1f},
            {1.f, 1.f, .1f},
            {.1f, 1.f, 1.f},
            {1.f, 1.f, 1.f} 
        };

        for(int i = 0; i < lightColors.size(); i++) {
            auto pointLight = LveGameObject::makePointLight(0.1f, 0.05f);
            pointLight.color = lightColors[i];
            auto rotateLight = glm::rotate(glm::mat4(1.0f), (i * glm::two_pi<float>()) / lightColors.size(), {0.0f, -1.0f, 0.0f});
            pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.0f, -1.0f, -1.0f, -1.0f));
            gameObjects.emplace(pointLight.getId(), std::move(pointLight));

        }
    }
} //namespace lve