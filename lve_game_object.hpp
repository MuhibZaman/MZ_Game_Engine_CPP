//Game object is anyting in a game that has a collction of properties and methods
#pragma once

#include "lve_model.hpp"

//libs
#include <glm/gtc/matrix_transform.hpp>

//std
#include <memory>
#include <unordered_map>

namespace lve {
    struct TransformComponent {
        glm::vec3 translation{0.0f, 0.0f, 0.f};
        glm::vec3 scale{1.0f, 1.0f, 1.0f};
        glm::vec3 rotation{0.0f, 0.0f, 0.f};

        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };

    //All game objects will be an instance of this one class
    class LveGameObject {
        public:
            using id_t = unsigned int;
            using Map = std::unordered_map<id_t, LveGameObject>;

            static LveGameObject createGameObject() {
                static id_t currentId = 0;
                return LveGameObject{currentId++};
            }

            LveGameObject(const LveGameObject &) = delete;
            LveGameObject &operator = (const LveGameObject &) = delete;
            LveGameObject(LveGameObject&&) = default;
            LveGameObject &operator = (LveGameObject&&) = default;

            id_t getId() {return id;}

            std::shared_ptr<LveModel> model{};
            glm::vec3 color{};
            TransformComponent transform{};

        private:
            LveGameObject(id_t objId) : id{objId} {}

            id_t id;
    };
} //namespace lve