//
// Created by micha on 16/01/2026.
//

#ifndef RAWENGINE_INSPECTOR_H
#define RAWENGINE_INSPECTOR_H
#include "EditorWindow.h"

namespace editor::editorWindows {
    class Inspector : public EditorWindow {
        // inherit constructors
        using EditorWindow::EditorWindow;

        void OnGUI() override;
    };
} // editor

#endif //RAWENGINE_INSPECTOR_H