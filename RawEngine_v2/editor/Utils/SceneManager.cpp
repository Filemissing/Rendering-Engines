//
// Created by micha on 01/11/2025.
//

#include "SceneManager.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <rttr/registration>

#include "../Editor.h"
using namespace nlohmann;
using namespace rttr;
using namespace core;

namespace editor {
    std::string SceneManager::basePath = "Assets/Scenes/";

    void SceneManager::SaveScene(const Scene* scene) {
        json sceneJson = json::object();
        sceneJson["name"] = scene->name;

        for (GameObject* obj : scene->objects) {
            sceneJson["objects"].push_back(obj->Serialize());
        }

        std::ofstream out(basePath + scene->name + ".json");
        out << sceneJson.dump(4);
    }

    Scene SceneManager::LoadScene(const std::string& name) {
        std::ifstream in(basePath + name + ".json");
        json sceneJson = json::parse(in);

        Scene scene = Scene(sceneJson["name"]);

        for (auto object : sceneJson["objects"]) {
            GameObject* gameObject = new GameObject(object["name"]);
            gameObject->Deserialize(object);
            scene.objects.push_back(gameObject);
        }

        printf("Loaded scene %s \n", name.c_str());

        return scene;
    }
} // editor