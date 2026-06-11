#include "first_app.hpp"
#include "simple_render_system.hpp"
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
    //Same thing as push constant data struct
    struct GlobalUbo {
        glm::mat4 projectionView{1.0f};
        glm::vec3 lightDirection = glm::normalize(glm::vec3{1.0f, -3.0f,-1.0f});  
    };

    FirstApp::FirstApp() {
        loadGameObjects();
    }

    FirstApp::~FirstApp() {}

    void FirstApp::run() {
        LveBuffer globalUboBuffer {
            lveDevice,
            sizeof(GlobalUbo),
            LveSwapChain::MAX_FRAMES_IN_FLIGHT,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            lveDevice.properties.limits.minUniformBufferOffsetAlignment
        };
        globalUboBuffer.map();

        SimpleRenderSystem simpleRenderSystem{lveDevice, lveRenderer.getSwapChainRenderPass()};
        LveCamera camera{};
        //camera.setViewDirection(glm::vec3(0.0f), glm::vec3(0.5f, 0.0f, 1.0f));
        camera.setViewTarget(glm::vec3(-1.0f, -2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 2.5f));

        auto viewerObject = LveGameObject::createGameObject(); //No model and wont be rendered. Just used to store camera state
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
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f); //fovy in radians
            if(auto commandBuffer = lveRenderer.beginFrame()) {
                int frameIndex = lveRenderer.getFrameIndex();
                //Setting up frameInfo
                FrameInfo frameInfo {
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera
                };

                //update objects phase
                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                globalUboBuffer.writeToIndex(&ubo, frameIndex);
                globalUboBuffer.flushIndex(frameIndex);

                //render phase
                lveRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
                lveRenderer.endSwapChainRenderPass(commandBuffer);
                lveRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(lveDevice.device());
    }

    void FirstApp::loadGameObjects() {
        // std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(lveDevice, "models/colored_cube.obj");
        // std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(lveDevice, "models/smooth_vase.obj");
        std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(lveDevice, "models/flat_vase.obj");
        auto gameObject = LveGameObject::createGameObject();

        gameObject.model = lveModel;
        gameObject.transform.translation = {0.0f, 0.5f, 2.5f};
        gameObject.transform.scale = glm::vec3(3.0f);

        gameObjects.push_back(std::move(gameObject));
    }
} //namespace lve