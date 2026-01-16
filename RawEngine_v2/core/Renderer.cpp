//
// Created by micha on 02/12/2025.
//

#include "Renderer.h"

#include <iostream>

#include "../editor/Editor.h"

namespace core {
    Renderer::Renderer() {

        // add effects to list
        auto* blur_horizontal = new PostProcessPass(new Material(
                        "Assets/shaders/PostProcessing/viewSpace.vert",
                        "Assets/shaders/PostProcessing/blur_horizontal.frag"));
        auto* blur_vertical = new PostProcessPass(new Material(
                        "Assets/shaders/PostProcessing/viewSpace.vert",
                        "Assets/shaders/PostProcessing/blur_vertical.frag"));
        postProcessingEffects.push_back(
            new PostProcessEffect("Bloom",
                {
                    new PostProcessPass(new Material(
                        "Assets/shaders/PostProcessing/viewSpace.vert",
                        "Assets/shaders/PostProcessing/bloom_threshold.frag"),
                        {PostProcessingParameter("_Threshold", typeid(float), 0.5f)}),
                    blur_horizontal,
                    blur_vertical,
                    blur_horizontal,
                    blur_vertical,
                    blur_horizontal,
                    blur_vertical,
                    blur_horizontal,
                    blur_vertical,
                    blur_horizontal,
                    blur_vertical,
                    blur_horizontal,
                    blur_vertical,
                    blur_horizontal,
                    blur_vertical,
                    blur_horizontal,
                    blur_vertical,
                    blur_horizontal,
                    blur_vertical,
                    blur_horizontal,
                    blur_vertical,
                    blur_horizontal,
                    blur_vertical,
                    blur_horizontal,
                    blur_vertical,
                    blur_horizontal,
                    blur_vertical,
                    blur_horizontal,
                    blur_vertical,
                    blur_horizontal,
                    blur_vertical,
                    blur_horizontal,
                    blur_vertical,
                    new PostProcessPass(new Material(
                        "Assets/shaders/PostProcessing/viewSpace.vert",
                        "Assets/shaders/PostProcessing/bloom_combine.frag"),
                        {PostProcessingParameter("_Intensity", typeid(float), 1.0f)})
                }
            )
        );

        postProcessingEffects.push_back(
            new PostProcessEffect("Hue Shift",
                {
                    new PostProcessPass(
                        new Material(
                            "Assets/shaders/PostProcessing/viewSpace.vert",
                            "Assets/shaders/PostProcessing/Hue shift.frag"),
                            {PostProcessingParameter("_Degrees", typeid(float), 0.0f)}
                    )
                }
            )
        );

        // postProcessingEffects.push_back(
        //     new PostProcessEffect("Invert",
        //         {
        //             new PostProcessPass(
        //                 new Material(
        //                     "Assets/shaders/PostProcessing/viewSpace.vert",
        //                     "Assets/shaders/PostProcessing/invert.frag"
        //                 )
        //             )
        //         }
        //     )
        // );
        //
        // postProcessingEffects.push_back(
        //     new PostProcessEffect("Test",
        //         {
        //             new PostProcessPass(
        //                 new Material(
        //                     "Assets/shaders/PostProcessing/viewSpace.vert",
        //                     "Assets/shaders/PostProcessing/test.frag"
        //                 )
        //             )
        //         }
        //     )
        // );

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
        glBindFramebuffer(GL_FRAMEBUFFER, sceneFbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneColor, 0);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        editor::Editor::activeScene->Render();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // blit scene to ppFbo[0]
        glBindFramebuffer(GL_READ_FRAMEBUFFER, sceneFbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ppFbo[0]);

        glBlitFramebuffer(
            0, 0, width, height,
            0, 0, width, height,
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST
        );

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        GLuint input = ppTex[0];
        GLuint depthTex = ppDepth[0];
        GLuint output = ppFbo[1];
        bool ping = true;
        // 2. Execute post-processing passes
        for (auto& effect : postProcessingEffects) {
            if (!effect->enabled)
                continue;

            for (auto pass : effect->passes)
            {
                output = ping ? ppFbo[1] : ppFbo[0];

                glBindFramebuffer(GL_FRAMEBUFFER, output);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // set input textures
                input = ping ? ppTex[0] : ppTex[1];
                pass->material->SetTexture("_MainTex", input);

                depthTex = ping ? ppDepth[0] : ppDepth[1];
                pass->material->SetTexture("_DepthTex", depthTex);


                pass->material->SetTexture("_SceneColor", sceneColor);
                pass->material->SetTexture("_SceneDepth", sceneDepth);

                pass->material->SetVec4("_TexelSize", glm::vec4(1.0f/float(width), 1.0f/float(height), 0, 0));

                for (PostProcessingParameter& param : pass->parameters) {
                    if (param.typeName == "float") {
                        pass->material->SetFloat(param.name, param.floatValue);
                    }
                    else if (param.typeName == "vec4") {
                        pass->material->SetVec4(param.name, param.vec4Value);
                    }
                    else if (param.typeName == "mat4") {
                        pass->material->SetMat4(param.name, param.mat4Value);
                    }
                }

                pass->material->Bind();
                quadModel->render();

                ping = !ping;

                glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbind
            }
        }

        // 3. Blit to final output
        glBindFramebuffer(GL_READ_FRAMEBUFFER, ping ? ppFbo[0] : ppFbo[1]);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, finalFbo);

        glBlitFramebuffer(
            0, 0, width, height,
            0, 0, width, height,
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST
        );

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

        // Create Scene buffers
        glGenFramebuffers(1, &sceneFbo);
        glGenTextures(1, &sceneColor);
        glGenRenderbuffers(1, &sceneDepth);

        // Create color texture storage
        glBindTexture(GL_TEXTURE_2D, sceneColor);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Optional wrap
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Bind FBO and attach the color texture
        glBindFramebuffer(GL_FRAMEBUFFER, sceneFbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneColor, 0);

        // Create + attach a dedicated depth+stencil renderbuffer for this FBO
        glBindRenderbuffer(GL_RENDERBUFFER, sceneDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, sceneDepth);

        // Tell GL which color attachments we'll draw into (required on some drivers)
        GLenum drawbuf = GL_COLOR_ATTACHMENT0;
        glDrawBuffers(1, &drawbuf);

        // Check completeness
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "[Renderer] sceneFBO " << " incomplete! status=0x" << std::hex << status << std::dec << std::endl;
        }



        // Create ping-pong FBOs + color textures + depth renderbuffers
        glGenFramebuffers(2, ppFbo);
        glGenTextures(2, ppTex);
        glGenRenderbuffers(2, ppDepth);

        for (int i = 0; i < 2; ++i) {
            // Create color texture storage
            glBindTexture(GL_TEXTURE_2D, ppTex[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
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
        glBindFramebuffer(GL_FRAMEBUFFER, prevFbo);
        glBindTexture(GL_TEXTURE_2D, prevTex);
        glBindRenderbuffer(GL_RENDERBUFFER, prevRbo);

        // Final error check
        while (GLenum e = glGetError()) {
            std::cerr << "GL error after EnsureFboSized(): 0x" << std::hex << e << std::dec << std::endl;
        }
    }
    void Renderer::DestroyFbo() {
        // Delete previous resources if they exist
        if (sceneFbo) glDeleteFramebuffers(1, &sceneFbo);
        if (sceneColor) glDeleteTextures(1, &sceneColor);
        if (sceneDepth) glDeleteRenderbuffers(1, &sceneDepth);

        if (ppFbo[0]) glDeleteFramebuffers(2, ppFbo);
        if (ppTex[0]) glDeleteTextures(2, ppTex);
        if (ppDepth[0]) glDeleteRenderbuffers(2, ppDepth);
    }
} // core