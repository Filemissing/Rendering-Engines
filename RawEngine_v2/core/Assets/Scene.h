//
// Created by micha on 20/10/2025.
//

#ifndef RAWENGINE_SCENE_H
#define RAWENGINE_SCENE_H
#include <string>
#include <vector>

#include "../Asset.h"
#include "../Components/Camera.h"
#include "../GameObject.h"

namespace core {
    class Light;

    class Scene : public Asset {
    public:
        std::string name;
        glm::vec4 ambientColor = glm::vec4(1);
        float ambientIntensity = 0.1f;

        Camera* mainCamera = nullptr;
        std::vector<Light*> lights;
        std::vector<GameObject*> objects;

        void Start();
        void Update();
        void Render();

        Scene(const std::string& name) : name(name) {};
        ~Scene();

        bool AddGameObject(GameObject* obj);
        void RemoveGameObject(GameObject* obj);
        void RemoveAndDeleteGameObject(GameObject* obj);

        GameObject* FindGameObjectByName(const std::string& name);
    };
}

#endif //RAWENGINE_SCENE_H