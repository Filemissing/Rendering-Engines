//
// Created by micha on 23/10/2025.
//

#ifndef RAWENGINE_TESTWINDOW_H
#define RAWENGINE_TESTWINDOW_H
#include "EditorWindow.h"

namespace editor::editorWindows {
    class TestWindow : public EditorWindow {
    public:
        // inherit constructors
        using EditorWindow::EditorWindow;

        void OnGUI() override;
    };
} // editorWindows

#endif //RAWENGINE_TESTWINDOW_H