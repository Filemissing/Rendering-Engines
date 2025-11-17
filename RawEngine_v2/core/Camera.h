//
// Created by micha on 13/10/2025.
//

#ifndef RAWENGINE_CAMERA_H
#define RAWENGINE_CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Component.h"

namespace core {
    class Camera : public Component {
    private:
        int width, height;

    public:
        static Camera* mainCamera;

        glm::mat4 getView();
        glm::mat4 projection;

        Camera() : Camera(1, 1) {}
        Camera(int width, int height);

        nlohmann::json Serialize() override;
        void Deserialize(const nlohmann::json&) override;
    };
    REGISTER_COMPONENT(Camera)
}

#endif //RAWENGINE_CAMERA_H