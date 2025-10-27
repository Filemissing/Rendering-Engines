//
// Created by micha on 20/10/2025.
//

#ifndef RAWENGINE_EDITORWINDOW_H
#define RAWENGINE_EDITORWINDOW_H
#include <string>
#include "imgui.h"


namespace editor::editorWindows {
    class EditorWindow {
        virtual void OnEnable();
        virtual void OnGUI();
    public:
        std::string name = "new Window";

        void Draw();

        explicit EditorWindow(const std::string& name);
        virtual ~EditorWindow() = default;
    };
} // editor

#endif //RAWENGINE_EDITORWINDOW_H