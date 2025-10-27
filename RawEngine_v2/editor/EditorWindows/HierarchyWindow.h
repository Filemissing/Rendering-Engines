//
// Created by micha on 23/10/2025.
//

#ifndef RAWENGINE_HIERARCHYWINDOW_H
#define RAWENGINE_HIERARCHYWINDOW_H
#include <unordered_map>

#include "EditorWindow.h"
#include "../UIElements/HierarchyNode.h"

namespace core {
    class GameObject;
}

namespace editor::editorWindows {
    class HierarchyWindow : public EditorWindow {
        // inherit constructors
        using EditorWindow::EditorWindow;

        void OnEnable() override;

        // stores foldoutStates of objects
        std::unordered_map<core::GameObject*, UIElements::HierarchyNode> hierarchyState;

        void DrawHierarchyNode(UIElements::HierarchyNode& node);
        void OnGUI() override;
    };
}

#endif //RAWENGINE_HIERARCHYWINDOW_H