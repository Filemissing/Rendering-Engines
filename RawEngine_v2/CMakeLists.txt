cmake_minimum_required(VERSION 3.29)
project(RawEngine)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(imgui imgui/backends)

# Add source files for ImGui
set(IMGUI_SOURCES
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_opengl3.cpp
        core/Assets/texture.cpp
        core/Assets/texture.h
        main.cpp
        main.cpp
        main.cpp
        main.cpp
        main.cpp
        "core/Components/Camera.cpp"
        "core/Components/Camera.h"
        "core/GameObject.cpp"
        "core/GameObject.h"
        "core/Components/Component.cpp"
        "core/Components/Component.h"
        "core/Components/Transform.cpp"
        "core/Components/Transform.h"
        "core/Components/MeshRenderer.cpp"
        "core/Components/MeshRenderer.h"
        core/Components/PlayerController.cpp
        core/Components/PlayerController.h
        core/Assets/Material.cpp
        core/Assets/Material.h
        core/Assets/Scene.cpp
        core/Assets/Scene.h
        core/Assets/RenderSettings.cpp
        core/Assets/RenderSettings.h
        editor/EditorWindows/EditorWindow.cpp
        editor/EditorWindows/EditorWindow.h
        editor/Editor.cpp
        editor/Editor.h
        editor/EditorWindows/TestWindow.cpp
        editor/EditorWindows/TestWindow.h
        editor/EditorWindows/HierarchyWindow.cpp
        editor/EditorWindows/HierarchyWindow.h
        editor/UIElements/HierarchyNode.cpp
        editor/UIElements/HierarchyNode.h
        editor/Utils/SceneManager.cpp
        editor/Utils/SceneManager.h
        core/ComponentFactory.cpp
        core/ComponentFactory.h
        editor/EditorWindows/SceneLoader.cpp
        editor/EditorWindows/SceneLoader.h
        core/Components/Light.cpp
        core/Components/Light.h
)

add_executable(RawEngine main.cpp
        ${IMGUI_SOURCES}
        core/assimpLoader.cpp
        core/assimpLoader.h
        core/Assets/mesh.cpp
        core/Assets/mesh.h
        core/vertex.h
        core/Assets/model.cpp
        core/Assets/model.h
        main.cpp
)

add_custom_command(
        TARGET RawEngine PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders/
        ${CMAKE_CURRENT_BINARY_DIR}/shaders/)

add_custom_command(
        TARGET RawEngine PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/models/
        ${CMAKE_CURRENT_BINARY_DIR}/models/)

add_custom_command(
        TARGET RawEngine PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/textures/
        ${CMAKE_CURRENT_BINARY_DIR}/textures/)

add_custom_target(copy_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        ${CMAKE_CURRENT_BINARY_DIR}/shaders
        DEPENDS ${SHADER_FILES}
        COMMENT "Copying shaders..."
)

add_dependencies(RawEngine copy_shaders)

find_package(glad CONFIG REQUIRED)
target_link_libraries(RawEngine PRIVATE glad::glad)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(RawEngine PRIVATE glfw)
find_package(glm CONFIG REQUIRED)
target_link_libraries(RawEngine PRIVATE glm::glm)
find_package(assimp CONFIG REQUIRED)
target_link_libraries(RawEngine PRIVATE assimp::assimp)
find_package(Stb REQUIRED)
target_include_directories(RawEngine PRIVATE ${Stb_INCLUDE_DIR})

# added by me, I think this simply includes the fonts directory into the final build - required for relative pathing
file(COPY fonts DESTINATION ${CMAKE_BINARY_DIR})

# add nlohmann
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(RawEngine PRIVATE nlohmann_json::nlohmann_json)

# add rttr
find_package(rttr CONFIG REQUIRED)
target_link_libraries(RawEngine PRIVATE RTTR::Core)