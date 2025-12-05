//
// Created by micha on 20/10/2025.
//

#ifndef RAWENGINE_EDITORWINDOW_H
#define RAWENGINE_EDITORWINDOW_H
#include <string>
#include "imgui.h"

namespace editor::editorWindows {
    class EditorWindow {
    public:
        std::string name = "new Window";
        bool isEnabled = true;

        bool hasFocus = false;
        bool isHovered = false;

        void Draw();

        virtual void OnEnable() {};
        virtual void OnGUI() {};

        explicit EditorWindow(const std::string& name) : name(name) {};
        virtual ~EditorWindow() = default;
    };
} // editor

#endif //RAWENGINE_EDITORWINDOW_H