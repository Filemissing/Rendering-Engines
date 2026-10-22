//
// Created by micha on 27/11/2025.
//

#ifndef RAWENGINE_VIEWPORT_H
#define RAWENGINE_VIEWPORT_H

#include <glad/glad.h>
#include "EditorWindow.h"
#include "../../core/Renderer.h"

namespace editor::editorWindows {
    class ViewPort : public EditorWindow {
    public:
        using EditorWindow::EditorWindow; // inherit constructor
        ~ViewPort() override;

        void OnEnable() override;
        void OnGUI() override;

        // Expose render target to app
        GLuint framebuffer() const { return m_fbo; }
        int width() const { return m_vpWidth; }
        int height() const { return m_vpHeight; }

        const core::Renderer* GetRenderer() const { return m_renderer; }
        core::Renderer* GetRenderer() { return m_renderer; }
        GLuint colorTexture() const { return m_colorTex; }

        // when x and y are > 0 the scene is rendered at this size instead of the window size (used by benchmarks)
        static glm::ivec2 forcedRenderSize;

    private:
        void EnsureFboSized(int w, int h);
        void DestroyFbo();

        GLuint m_fbo = 0;
        GLuint m_colorTex = 0;
        GLuint m_depthRb = 0;
        int m_vpWidth = 100, m_vpHeight = 100;

        core::Renderer* m_renderer = nullptr;
    };
} // editor

#endif //RAWENGINE_VIEWPORT_H