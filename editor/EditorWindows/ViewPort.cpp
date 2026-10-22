//
// Created by micha on 27/11/2025.
//

#include "ViewPort.h"
#include <iostream>
#include "../Editor.h"

namespace editor::editorWindows {
    ViewPort::~ViewPort() {
        DestroyFbo();

        delete m_renderer;
    }

    void ViewPort::OnEnable() {
        Editor::viewPort = this;

        m_renderer = new core::Renderer();
    }

    void ViewPort::EnsureFboSized(int w, int h) {
        // Validate size
        if (w <= 0 || h <= 0) return;
        if (m_vpWidth == w && m_vpHeight == h && m_fbo) return;

        // Destroy existing FBO
        DestroyFbo();

        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        glGenTextures(1, &m_colorTex);
        glBindTexture(GL_TEXTURE_2D, m_colorTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTex, 0);

        glGenRenderbuffers(1, &m_depthRb);
        glBindRenderbuffer(GL_RENDERBUFFER, m_depthRb);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthRb);

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "[GUI] Framebuffer incomplete! Status = 0x" << std::hex << status << std::dec << std::endl;
            DestroyFbo();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_renderer->EnsureFboSized(w, h);

        // Store new size
        m_vpWidth = w;
        m_vpHeight = h;
    }

    void ViewPort::DestroyFbo() {
        if (m_colorTex) { glDeleteTextures(1, &m_colorTex);		m_colorTex = 0; }
        if (m_depthRb) { glDeleteRenderbuffers(1, &m_depthRb);	m_depthRb = 0; }
        if (m_fbo) { glDeleteFramebuffers(1, &m_fbo);		        m_fbo = 0; }
        m_vpWidth = m_vpHeight = 0;
    }

    void ViewPort::OnGUI() {
        if (!isEnabled) return;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        ImVec2 avail = ImGui::GetContentRegionAvail();
        int w = (int)avail.x;
        int h = (int)avail.y;
        EnsureFboSized(w, h);

        if (core::Camera::GetMainCamera())
            core::Camera::GetMainCamera()->RecalculateProjection(m_vpWidth, m_vpHeight);

        hasFocus = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
        isHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);

        GLuint fb = framebuffer();
        if (m_vpWidth > 0 && m_vpHeight > 0 && fb != 0) {

            // render the scene into the FBO
            if (Editor::activeScene) {
                m_renderer->RenderScene(Editor::activeScene, fb, m_colorTex, m_vpWidth, m_vpHeight);
            }

            while (GLenum e = glGetError()) std::cerr << "GLERR: " << std::hex << e << std::dec << std::endl;

            // unbind
            glViewport(0, 0, Editor::GetMainWindowSize().x, Editor::GetMainWindowSize().y);
        }

        // Draw the color attachment (flip v)
        if (m_colorTex) {
            ImGui::Image((ImTextureID)(intptr_t)m_colorTex, avail, ImVec2(0, 1), ImVec2(1, 0));
        }

        ImGui::PopStyleVar();
    }
} // editor