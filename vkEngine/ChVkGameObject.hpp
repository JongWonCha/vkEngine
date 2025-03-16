#pragma once

#include "ChVkModel.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace chVk
{
    struct TransformComponent
    {
        glm::vec3 translation{}; // position offset
        glm::vec3 scale{ 1.f, 1.f, 1.f };
        glm::vec3 rotation{};
        glm::mat4 mat4() // homogeneous matrix
        {
            // transform : 
            // 1 0 0 tx
            // 0 1 0 ty
            // 0 0 1 tz
            // 0 0 0 1
            auto transform = glm::translate(glm::mat4(1.0f), translation);

            transform = glm::rotate(transform, rotation.y, {0.f, 1.f, 0.f} );
            transform = glm::rotate(transform, rotation.x, {1.f, 0.f, 0.f} );
            transform = glm::rotate(transform, rotation.z, {0.f, 0.f, 1.f} );
            
            // transform *  sx  0   0   0
            //              0   sy  0   0
            //              0   0   sz  0
            //              0   0   0   1
            transform = glm::scale(transform, scale); // transform * scale

            return transform;
        }
    };
    
    class ChVkGameObject
    {
    public:
        using id_t = unsigned int;

        static ChVkGameObject CreateGameObject()
        {
            static id_t currentId = 0;
            return ChVkGameObject{currentId++};
        }

        ChVkGameObject(const ChVkGameObject&) = delete;
        ChVkGameObject& operator=(const ChVkGameObject&) = delete;
        ChVkGameObject(ChVkGameObject&&) = default;
        ChVkGameObject& operator=(ChVkGameObject&&) = default;

        id_t GetId() const { return _id; }

        std::shared_ptr<ChVkModel> _model;
        glm::vec3 _color;
        TransformComponent _transform{};
        
    private:
        ChVkGameObject(id_t objId) : _id(objId) {}
        id_t _id;
    };
}
