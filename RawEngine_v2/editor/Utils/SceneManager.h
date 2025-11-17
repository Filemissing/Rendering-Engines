//
// Created by micha on 01/11/2025.
//

#ifndef RAWENGINE_SCENEMANAGER_H
#define RAWENGINE_SCENEMANAGER_H
#include "../../core/Scene.h"

namespace editor {
    class SceneManager {
    public:
        static std::string basePath;
        static void SaveScene(const core::Scene* scene);
        static core::Scene LoadScene(const std::string& name);
    };
} // editor

#endif //RAWENGINE_SCENEMANAGER_H