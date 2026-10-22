//
// Created by micha on 01/12/2025.
//

#ifndef RAWENGINE_TRANSFORMWINDOW_H
#define RAWENGINE_TRANSFORMWINDOW_H
#include "EditorWindow.h"

namespace editor::editorWindows {
    class TransformWindow : public EditorWindow {
    public:
        using EditorWindow::EditorWindow;

        void OnGUI() override;
    };
} // editor

#endif //RAWENGINE_TRANSFORMWINDOW_H