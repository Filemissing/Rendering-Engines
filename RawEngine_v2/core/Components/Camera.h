//
// Created by micha on 13/10/2025.
//

#ifndef RAWENGINE_CAMERA_H
#define RAWENGINE_CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Component.h"
#include "../GameObject.h"

namespace core {
    class Camera : public Component {
    private:
        int width, height;

    public:
        glm::mat4 getView();
        glm::mat4 projection;

        Camera() : Camera(1, 1) {}
        explicit Camera(GameObject* gameObject) : Camera(1, 1, gameObject) {gameObject->AddComponent(this);};
        Camera(int width, int height, GameObject* gameObject = nullptr);
        ~Camera() override;

        static Camera* GetMainCamera();

        nlohmann::json Serialize() override;
        void Deserialize(const nlohmann::json&) override;
    };
    REGISTER_COMPONENT(Camera)
}

#endif //RAWENGINE_CAMERA_H