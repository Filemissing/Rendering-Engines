//
// Created by micha on 20/10/2025.
//

#ifndef RAWENGINE_SCENE_H
#define RAWENGINE_SCENE_H
#include <string>
#include <vector>

#include "GameObject.h"

namespace core {
    class Scene {
    public:
        std::string name;

        std::vector<GameObject*> objects;

        void Start();
        void Update();

        Scene(const std::string& name) : name(name) {};
        ~Scene();

        bool AddGameObject(GameObject* obj);
        void RemoveGameObject(GameObject* obj);
        void RemoveAndDeleteGameObject(GameObject* obj);
    };
}

#endif //RAWENGINE_SCENE_H