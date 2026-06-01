#include "first_app.hpp"
#include "simple_render_system.hpp"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <stdexcept>
#include <array>
#include <iostream>

namespace lve {
    FirstApp::FirstApp() {
        loadGameObjects();
    }

    FirstApp::~FirstApp() {}

    void FirstApp::run() {
        SimpleRenderSystem simpleRenderSystem{lveDevice, lveRenderer.getSwapChainRenderPass()};

        while(!lveWindow.shouldClose()) {
            //Checks for events done on the window
            glfwPollEvents();
            if(auto commandBuffer = lveRenderer.beginFrame()) {
                lveRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
                lveRenderer.endSwapChainRenderPass(commandBuffer);
                lveRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(lveDevice.device());
    }

    void fractal(std::vector<LveModel::Vertex> *vertices, LveModel::Vertex one, LveModel::Vertex two, LveModel::Vertex three, int depth) {
        if(depth == 5) {
            vertices->push_back(one);
            vertices->push_back(two);
            vertices->push_back(three);
            return;
        }
        
        float newOne_x = (one.position[0] + two.position[0]) / 2.0f;
        float newOne_y = (one.position[1] + two.position[1]) / 2.0f;
        LveModel::Vertex midOne = {{newOne_x, newOne_y}, {1.0f, 0.0f, 0.0f}};

        float newTwo_x = (two.position[0] + three.position[0]) / 2.0f;
        float newTwo_y = (two.position[1] + three.position[1]) / 2.0f;
        LveModel::Vertex midTwo = {{newTwo_x, newTwo_y}, {0.0f, 1.0f, 0.0f}};

        float newThree_x = (three.position[0] + one.position[0]) / 2.0f;
        float newThree_y = (three.position[1] + one.position[1]) / 2.0f;
        LveModel::Vertex midThree = {{newThree_x, newThree_y}, {0.0f, 0.0f, 1.0f}};

        fractal(vertices, one, midOne, midThree, depth + 1);
        fractal(vertices, midOne, two, midTwo, depth + 1);
        fractal(vertices, midThree, midTwo, three, depth + 1);
    }

    void FirstApp::loadGameObjects() {
        std::vector<LveModel::Vertex> vertices;
        fractal(&vertices,
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            0
        );

        std::vector<LveModel::Vertex> tmpVertices;
        int i = 0;
        for(auto &vertex : vertices) {
            tmpVertices.push_back(vertex);
            i++;
            if(i == 3) {
                auto lveModel = std::make_shared<LveModel>(lveDevice, tmpVertices); //One model instance used by multiple game objects

                auto triangle = LveGameObject::createGameObject();
                triangle.model = lveModel;
                triangle.color = {0.1f, 0.8f, 0.1f};
                // triangle.transform2d.translation;
                // triangle.transform2d.scale;
                triangle.transform2d.rotation = 0.25f * glm::two_pi<float>(); //Using radians

                gameObjects.push_back(std::move(triangle));

                i = 0;
                tmpVertices.clear();
            }
        }
    }
} //namespace lve