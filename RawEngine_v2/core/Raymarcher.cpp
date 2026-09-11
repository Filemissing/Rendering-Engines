//
// Created by micha on 09/09/2026.
//

#include "Raymarcher.h"

#include <iostream>

#include "../editor/Editor.h"

namespace core {
    Raymarcher::Raymarcher() {
        volumeTex = 0;
        bakeFbo = 0;

        bakeMaterial = new Material(
            "Assets/shaders/PostProcessing/viewSpace.vert",
            "Assets/shaders/raymarching/bake.frag");
        marchMaterial = new Material(
            "Assets/shaders/PostProcessing/viewSpace.vert",
            "Assets/shaders/raymarching/march.frag");

        std::vector<Mesh> meshes = std::vector<Mesh>();
        meshes.push_back(Mesh::generateQuad());
        quadModel = new Model(std::move(meshes));

        EnsureVolumeSized(glm::ivec3(128, 128, 128));
        SetWorldBounds(glm::vec3(-30, -30, -30), glm::vec3(30, 30, 30));
    }
    Raymarcher::~Raymarcher() {
        DestroyFbo();
        delete bakeMaterial;
        delete marchMaterial;
    }

    void Raymarcher::EnsureVolumeSized(glm::ivec3 newResolution) {
        resolution = newResolution;

        DestroyFbo();
        glGenTextures(1, &volumeTex);
        glGenFramebuffers(1, &bakeFbo);


        glBindTexture(GL_TEXTURE_3D, volumeTex);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, resolution.x, resolution.y, resolution.z, 0, GL_RED, GL_HALF_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


        // // Bind FBO and attach the color texture
        // glBindFramebuffer(GL_FRAMEBUFFER, bakeFbo);
        // GLenum drawbuf = GL_COLOR_ATTACHMENT0;
        // glDrawBuffers(1, &drawbuf);

        // Check completeness
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "[Raymarcher] bakeFbo " << " incomplete! status=0x" << std::hex << status << std::dec << std::endl;
        }

        // Final error check
        while (GLenum e = glGetError()) {
            std::cerr << "GL error after EnsureFboSized(): 0x" << std::hex << e << std::dec << std::endl;
        }
    }
    void Raymarcher::DestroyFbo() {
        if (volumeTex) glDeleteTextures(1, &volumeTex);
        if (bakeFbo) glDeleteFramebuffers(1, &bakeFbo);
    }

    void UploadPrimitives(GLuint shader, const std::vector<Primitive*>& primitives) {
        glUseProgram(shader);

        for (int i = 0; i < primitives.size(); i++) {
            const Primitive* primitive = primitives[i];

            std::string base = "primitives[" + std::to_string(i) + "].";

            glUniform1i(glGetUniformLocation(shader, (base + "type").c_str()),
                primitive->type);

            glUniform3fv(glGetUniformLocation(shader, (base + "position").c_str()),
                1, glm::value_ptr(primitive->gameObject->transform.position));

            glUniform3fv(glGetUniformLocation(shader, (base + "rotation").c_str()),
                1, glm::value_ptr(primitive->gameObject->transform.rotation));

            glUniform3fv(glGetUniformLocation(shader, (base + "scale").c_str()),
                1, glm::value_ptr(primitive->gameObject->transform.scale));

            glUniform3fv(glGetUniformLocation(shader, (base + "halfExtents").c_str()),
                1, glm::value_ptr(primitive->halfExtents));

            glUniform3fv(glGetUniformLocation(shader, (base + "color").c_str()),
                1, glm::value_ptr(primitive->color));
        }

        GLint loc = glGetUniformLocation(shader, "primitiveCount");
        if (loc == -1) return;
        glUniform1i(loc, primitives.size());
    }
    void Raymarcher::Bake(const std::vector<Primitive*>& primitives) {
        printf("Baking %zu primitives\n", primitives.size());
        glBindFramebuffer(GL_FRAMEBUFFER, bakeFbo);

        glDisable(GL_DEPTH_TEST);

        GLint prevViewport[4];
        glGetIntegerv(GL_VIEWPORT, prevViewport);
        glViewport(0, 0, resolution.x, resolution.y);

        bakeMaterial->SetVec3("_WorldMin", worldMin);
        bakeMaterial->SetVec3("_WorldMax", worldMax);

        bakeMaterial->SetInt("_ResolutionZ", resolution.z);

        UploadPrimitives(bakeMaterial->GetShaderProgram(), primitives);
        bakeMaterial->Bind();

        GLint sliceLoc = glGetUniformLocation(bakeMaterial->GetShaderProgram(), "_SliceZ");
        for (int z = 0; z < resolution.z; z++) {
            glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, volumeTex, 0, z);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUniform1i(sliceLoc, z);
            quadModel->render();
        }

        glEnable(GL_DEPTH_TEST);
        glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        dirty = false;
    }

    void Raymarcher::Render(GLuint targetFbo, GLuint sceneColorTex, GLuint sceneDepthTex, Camera* cam) {
        glBindFramebuffer(GL_FRAMEBUFFER, targetFbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_3D, volumeTex);
        std::vector<float> data(resolution.x * resolution.y * resolution.z);
        glGetTexImage(GL_TEXTURE_3D, 0, GL_RED, GL_FLOAT, data.data());
        printf("pixel: %f, %f, %f\n", data[99], data[100], data[101]);

        marchMaterial->SetTexture2D("_MainTex", sceneColorTex);
        marchMaterial->SetTexture2D("_SceneDepth", sceneDepthTex);
        marchMaterial->SetTexture3D("_VolumeTex", volumeTex);

        marchMaterial->SetVec3("_WorldMin", worldMin);
        marchMaterial->SetVec3("_WorldMax", worldMax);

        marchMaterial->SetMat4("_InvView", glm::inverse(cam->GetView()));
        marchMaterial->SetMat4("_InvProj", glm::inverse(cam->GetProjection()));
        marchMaterial->SetVec3("_CameraPos", cam->gameObject->transform.position);
        marchMaterial->SetFloat("_Far", cam->farPlane);

        marchMaterial->SetInt("_MaxSteps", maxSteps);
        marchMaterial->SetFloat("_MaxDist", maxDist);
        marchMaterial->SetFloat("_SurfDist", surfDist);

        marchMaterial->Bind();
        quadModel->render();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Raymarcher::SetWorldBounds(glm::vec3 min, glm::vec3 max) {
        worldMin = min;
        worldMax = max;
        MarkDirty();
    }

} // core