//
// Created by micha on 17/11/2025.
//

#ifndef RAWENGINE_SCENELOADER_H
#define RAWENGINE_SCENELOADER_H
#include "EditorWindow.h"

namespace editor::editorWindows {
    class SceneLoader : public EditorWindow {
        // inherit constructors
        using EditorWindow::EditorWindow;

        std::string sceneName;

        void OnEnable() override;

        void OnGUI() override;
    };
} // editor

#endif //RAWENGINE_SCENELOADER_H