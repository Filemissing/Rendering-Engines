//
// Created by micha on 20/10/2025.
//

#include "Scene.h"

namespace core {
    void Scene::Start() {
        for (auto object : objects) {
            object->Start();
        }
    }

    void Scene::Update() {
        for (auto object : objects) {
            object->Update();
        }
    }

    Scene::~Scene() {
        for (auto object : objects) {
            delete object;
        }
    }
}