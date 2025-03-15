#pragma once

#include "chVkModel.hpp"

#include <memory>

namespace chVk
{
    struct Transform2dComponent
    {
        glm::vec2 translation{}; // position offset
        glm::vec2 scale{ 1.f, 1.f };
        float rotation;
        glm::mat2 mat2()
        {
            const float s = glm::sin(rotation);
            const float c = glm::cos(rotation);
            glm::mat2 rotMatrix{ {c, s}, {-s, c} };
            glm::mat2 scaleMat{ {scale.x, .0f}, {.0f, scale.y} };
            return rotMatrix * scaleMat;
        }
    };
    
    class chVkGameObject
    {
    public:
        using id_t = unsigned int;

        static chVkGameObject CreateGameObject()
        {
            static id_t currentId = 0;
            return chVkGameObject{currentId++};
        }

        chVkGameObject(const chVkGameObject&) = delete;
        chVkGameObject& operator=(const chVkGameObject&) = delete;
        chVkGameObject(chVkGameObject&&) = default;
        chVkGameObject& operator=(chVkGameObject&&) = default;

        id_t GetId() const { return _id; }

        std::shared_ptr<chVkModel> _model;
        glm::vec3 _color;
        Transform2dComponent _transform2d{};
        
    private:
        chVkGameObject(id_t objId) : _id(objId) {}
        id_t _id;
    };
}
