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

    bool Scene::AddGameObject(GameObject *obj) {
        if (!obj) return false;
        if (obj->scene != nullptr) {
            printf("object is already part of a different scene");
            return false;
        }

        auto it = std::find(objects.begin(), objects.end(), obj);
        if (it == objects.end()) {
            obj->scene = this;
            obj->transform.root = &obj->transform;
            objects.push_back(obj);
            return true;
        }
        return false;
    }
    void Scene::RemoveGameObject(GameObject* obj) {
        objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
    }
    void Scene::RemoveAndDeleteGameObject(GameObject* obj) {
        if (std::find(objects.begin(), objects.end(), obj) == objects.end()) return;

        std::erase(objects, obj); // remove pointer
        delete obj;
    }

    GameObject* Scene::FindGameObjectByName(const std::string& name)
    {
        for (auto* go : objects)
            if (go && go->name == name)
                return go;

        return nullptr;
    }
}
