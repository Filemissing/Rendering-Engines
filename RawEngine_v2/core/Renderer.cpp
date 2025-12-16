//
// Created by micha on 02/12/2025.
//

#include "Renderer.h"

#include <iostream>

#include "../editor/Editor.h"

namespace core {
    PostProcessEffect::PostProcessEffect(const std::string& name, Material* material) : name(name), material(material) {}
    PostProcessEffect::~PostProcessEffect() {
        delete material;
    }

    Renderer::Renderer() :
        defaultMaterial(new Material("Assets/shaders/PostProcessing/viewSpace.vert", "Assets/shaders/texture.frag"))
    {

        // add effects to list
        postProcessingEffects.push_back(new PostProcessEffect("Invert",
            new Material(
                "Assets/shaders/PostProcessing/viewSpace.vert",
                "Assets/shaders/PostProcessing/invert.frag"
                )
            )
        );

        std::vector<Mesh> meshes = std::vector<Mesh>();
        meshes.push_back(Mesh::generateQuad());
        quadModel = new Model(std::move(meshes));

        ppFbo[0] = ppFbo[1] = 0;
        ppTex[0] = ppTex[1] = 0;
        ppDepth[0] = ppDepth[1] = 0;
    }
    Renderer::~Renderer() {
        DestroyFbo();
        for (auto& pass : postProcessingEffects) {
            delete pass;
        }
    }

    void Renderer::RenderScene(Scene*& scene, const GLuint& finalFbo, const GLuint& finalTexture, int width, int height) {
        // 1. Render scene into pingFbo[0]
        glBindFramebuffer(GL_FRAMEBUFFER, ppFbo[0]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ppTex[0], 0);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        editor::Editor::activeScene->Render();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        GLuint input = ppTex[0];
        GLuint depthTex = ppDepth[0];
        GLuint output = ppFbo[1];
        bool ping = true;
        // 2. Execute post-processing passes
        for (auto& pass : postProcessingEffects) {
            if (!pass->enabled)
                return;

            output = ping ? ppFbo[1] : ppFbo[0];

            glBindFramebuffer(GL_FRAMEBUFFER, output);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);

            // set input textures
            input = ping ? ppTex[0] : ppTex[1];
            pass->material->SetTexture("_MainTex", input);

            depthTex = ping ? ppDepth[0] : ppDepth[1];
            pass->material->SetTexture("_DepthTex", depthTex);

            pass->material->Bind();

            quadModel->render();

            ping = !ping;

            glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbind
        }

        // 3. Blit to final output
        glBindFramebuffer(GL_FRAMEBUFFER, finalFbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, finalTexture, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, width, height);

        input = ping ? ppTex[0] : ppTex[1];
        defaultMaterial->SetTexture("_MainTex", input);
        defaultMaterial->Bind();
        quadModel->render();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Renderer::EnsureFboSized(int width, int height) {
        if (width <= 0 || height <= 0) return;

        // Save previous bindings so we restore them and don't disturb other code
        GLint prevFbo = 0, prevTex = 0, prevRbo = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFbo);
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTex);
        glGetIntegerv(GL_RENDERBUFFER_BINDING, &prevRbo);

        // Destroy previous resources (if any)
        DestroyFbo();

        // Create ping-pong FBOs + color textures + depth renderbuffers
        glGenFramebuffers(2, ppFbo);
        glGenTextures(2, ppTex);
        glGenRenderbuffers(2, ppDepth);
        // ppDepth are renderbuffer IDs we declared earlier (GLuint ppDepth[2] = {0,0};)

        for (int i = 0; i < 2; ++i) {
            // Create color texture storage
            glBindTexture(GL_TEXTURE_2D, ppTex[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // Optional wrap
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Bind FBO and attach the color texture
            glBindFramebuffer(GL_FRAMEBUFFER, ppFbo[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ppTex[i], 0);

            // Create + attach a dedicated depth+stencil renderbuffer for this FBO
            glBindRenderbuffer(GL_RENDERBUFFER, ppDepth[i]);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ppDepth[i]);

            // Tell GL which color attachments we'll draw into (required on some drivers)
            GLenum drawbuf = GL_COLOR_ATTACHMENT0;
            glDrawBuffers(1, &drawbuf);

            // Check completeness
            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE) {
                std::cerr << "[Renderer] FBO " << i << " incomplete! status=0x" << std::hex << status << std::dec << std::endl;
            }
        }

        // Unbind everything we touched and restore previous bindings
        glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)prevFbo);
        glBindTexture(GL_TEXTURE_2D, (GLuint)prevTex);
        glBindRenderbuffer(GL_RENDERBUFFER, (GLuint)prevRbo);

        // Clear GL error state (optional) and return v
        while (GLenum e = glGetError()) {
            std::cerr << "GL error after EnsureFboSized(): 0x" << std::hex << e << std::dec << std::endl;
        }
    }
    void Renderer::DestroyFbo() {
        // Delete previous resources if they exist
        if (ppFbo[0]) glDeleteFramebuffers(2, ppFbo);
        if (ppTex[0]) glDeleteTextures(2, ppTex);
        if (ppDepth[0]) glDeleteRenderbuffers(2, ppDepth);
    }



} // core