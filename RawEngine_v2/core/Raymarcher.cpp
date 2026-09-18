//
// Created by micha on 09/09/2026.
//

#include "Raymarcher.h"

#include <iostream>

#include "../editor/Editor.h"
#include "../editor/EditorWindows/BenchmarkTool.h"

namespace core {
    Raymarcher::Raymarcher() {
        glGenQueries(1, &m_timeQuery);

        volumeTex = 0;

        marchMaterial = new Material(
            "Assets/shaders/PostProcessing/viewSpace.vert",
            "Assets/shaders/raymarching/march.frag");
        debugSliceMaterial = new Material(
            "Assets/shaders/PostProcessing/viewSpace.vert",
            "Assets/shaders/debug/slice_view.frag");

        classifyShader = new ComputeShader("Assets/shaders/raymarching/classify.comp");
        jfaStepShader = new ComputeShader("Assets/shaders/raymarching/jfaStep.comp");
        jfaFinalizeShader = new ComputeShader("Assets/shaders/raymarching/jfaFinalize.comp");

        std::vector<Mesh> meshes = std::vector<Mesh>();
        meshes.push_back(Mesh::generateQuad());
        quadModel = new Model(std::move(meshes));

        EnsureVolumeSized(glm::ivec3(128, 128, 128));
        SetWorldBounds(glm::vec3(-30, -30, -30), glm::vec3(30, 30, 30));
    }
    Raymarcher::~Raymarcher() {
        DestroyFbo();
        delete marchMaterial;
        delete quadModel;

        delete classifyShader;
        delete jfaStepShader;
        delete jfaFinalizeShader;
    }

    void Raymarcher::EnsureVolumeSized(glm::ivec3 newResolution) {
        resolution = newResolution;
        DestroyFbo();
        EnsureJFAResourcesSized();

        glGenTextures(1, &volumeTex);

        glBindTexture(GL_TEXTURE_3D, volumeTex);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, resolution.x, resolution.y, resolution.z, 0, GL_RED, GL_HALF_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        // Final error check
        while (GLenum e = glGetError()) {
            std::cerr << "GL error after EnsureFboSized(): 0x" << std::hex << e << std::dec << std::endl;
        }
    }
    void Raymarcher::EnsureJFAResourcesSized() {
        // ping-pong textures
        glGenTextures(1, &jfaTexA);
        glBindTexture(GL_TEXTURE_3D, jfaTexA);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, resolution.x, resolution.y, resolution.z,
                     0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // NEAREST — these are indices, not colors to blend
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glGenTextures(1, &jfaTexB);
        glBindTexture(GL_TEXTURE_3D, jfaTexB);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, resolution.x, resolution.y, resolution.z,
                     0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        // sign/occupancy, single channel, persists across JFA passes (never ping-ponged)
        glGenTextures(1, &signTex);
        glBindTexture(GL_TEXTURE_3D, signTex);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, resolution.x, resolution.y, resolution.z,
                     0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    void Raymarcher::DestroyFbo() {
        if (volumeTex) glDeleteTextures(1, &volumeTex);
        if (jfaTexA) glDeleteTextures(1, &jfaTexA);
        if (jfaTexB) glDeleteTextures(1, &jfaTexB);
        if (signTex) glDeleteTextures(1, &signTex);
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

        if (!m_queryInFlight) {
            glBeginQuery(GL_TIME_ELAPSED, m_timeQuery);
        }

        const int gx = (resolution.x + 7) / 8;
        const int gy = (resolution.y + 7) / 8;
        const int gz = (resolution.z + 7) / 8;

        // ---- Pass 1: classify ----
        // writes each voxel's OWN position into jfaTexA if it's a seed (a surface-adjacent voxel),
        // or an invalid marker (w=0) otherwise
        classifyShader->SetVec3("_WorldMin", worldMin);
        classifyShader->SetVec3("_WorldMax", worldMax);
        classifyShader->SetVec3("_Resolution", resolution);
        classifyShader->SetInt("_Octaves", octaves);
        classifyShader->SetFloat("_WarpStrength", warpStrength);
        classifyShader->SetFloat("_Lacunarity", lacunarity);
        classifyShader->SetFloat("_Persistence", persistence);
        classifyShader->SetFloat("_HeightScale", heightScale);
        UploadPrimitives(classifyShader->GetProgram(), editor::Editor::activeScene->primitives);
        classifyShader->Bind();
        classifyShader->BindImage(0, jfaTexA, GL_WRITE_ONLY, GL_RGBA32F);
        classifyShader->BindImage(1, signTex, GL_WRITE_ONLY, GL_R8);
        classifyShader->Dispatch(gx, gy, gz);

        // ---- Pass 2: JFA step iterations, ping-ponging A<->B ----
        // step size halves each pass: N/2, N/4, ..., 1
        bool pingIsA = true;
        int step = std::max({resolution.x, resolution.y, resolution.z}) / 2;
        { // 1+JFA extra pass
            GLuint src = pingIsA ? jfaTexA : jfaTexB;
            GLuint dst = pingIsA ? jfaTexB : jfaTexA;

            jfaStepShader->SetInt("_Step", 1);
            jfaStepShader->SetVec3("_Resolution", resolution);
            jfaStepShader->Bind();
            jfaStepShader->BindImage(0, src, GL_READ_ONLY, GL_RGBA32F);
            jfaStepShader->BindImage(1, dst, GL_WRITE_ONLY, GL_RGBA32F);
            jfaStepShader->Dispatch(gx, gy, gz);

            pingIsA = !pingIsA;
        }
        while (step >= 1) {
            GLuint src = pingIsA ? jfaTexA : jfaTexB;
            GLuint dst = pingIsA ? jfaTexB : jfaTexA;

            jfaStepShader->SetInt("_Step", step);
            jfaStepShader->SetVec3("_Resolution", resolution);
            jfaStepShader->Bind();
            jfaStepShader->BindImage(0, src, GL_READ_ONLY, GL_RGBA32F);
            jfaStepShader->BindImage(1, dst, GL_WRITE_ONLY, GL_RGBA32F);
            jfaStepShader->Dispatch(gx, gy, gz);

            pingIsA = !pingIsA;
            step /= 2;
        }
        // // JFA+2 extra passes
        // for (int extra = 2; extra > 0; extra--) {
        //     GLuint src = pingIsA ? jfaTexA : jfaTexB;
        //     GLuint dst = pingIsA ? jfaTexB : jfaTexA;
        //
        //     jfaStepShader->SetInt("_Step", extra);
        //     jfaStepShader->SetVec3("_Resolution", resolution);
        //     jfaStepShader->Bind();
        //     jfaStepShader->BindImage(0, src, GL_READ_ONLY, GL_RGBA32F);
        //     jfaStepShader->BindImage(1, dst, GL_WRITE_ONLY, GL_RGBA32F);
        //     jfaStepShader->Dispatch(gx, gy, gz);
        //
        //     pingIsA = !pingIsA;
        // }
        finalSeedTex = pingIsA ? jfaTexA : jfaTexB;

        finalSeedTex = pingIsA ? jfaTexA : jfaTexB;

        // ---- Pass 3: finalize — convert nearest-seed position into signed distance, write to volumeTex ----
        jfaFinalizeShader->SetVec3("_WorldMin", worldMin);
        jfaFinalizeShader->SetVec3("_WorldMax", worldMax);
        jfaFinalizeShader->SetVec3("_Resolution", resolution);
        jfaFinalizeShader->Bind();
        jfaFinalizeShader->BindImage(0, finalSeedTex, GL_READ_ONLY, GL_RGBA32F);
        jfaFinalizeShader->BindImage(1, signTex, GL_READ_ONLY, GL_R8);
        jfaFinalizeShader->BindImage(2, volumeTex, GL_WRITE_ONLY, GL_R16F);
        jfaFinalizeShader->Dispatch(gx, gy, gz);

        dirty = false;

        if (!m_queryInFlight) {
            glEndQuery(GL_TIME_ELAPSED);
            m_queryInFlight = true;
        }
    }

    void Raymarcher::Render(GLuint targetFbo, GLuint sceneColorTex, GLuint sceneDepthTex, Camera* cam) {
        if (m_queryInFlight) {
            GLint available = 0;
            glGetQueryObjectiv(m_timeQuery, GL_QUERY_RESULT_AVAILABLE, &available);
            if (available) {
                GLuint64 elapsedNs = 0;
                glGetQueryObjectui64v(m_timeQuery, GL_QUERY_RESULT, &elapsedNs);
                editor::editorWindows::BenchmarkTool::PushSample("Bake time",
                    static_cast<double>(elapsedNs) * 1e-6);
                m_queryInFlight = false;
            }
        }

        if (debug) {
            GLuint dTex = 0;
            if (debugTex == dSignTex) dTex = signTex;
            if (debugTex == dSeedTex) dTex = finalSeedTex;
            if (debugTex == dVolumeTex) dTex = volumeTex;
            RenderDebugSlice(targetFbo, dTex);
            return;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, targetFbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_3D, volumeTex);
        // std::vector<float> data(resolution.x * resolution.y * resolution.z);
        // glGetTexImage(GL_TEXTURE_3D, 0, GL_RED, GL_FLOAT, data.data());
        // printf("pixel: %f, %f, %f\n", data[99], data[100], data[101]);

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
    // Raymarcher.cpp
    void Raymarcher::RenderDebugSlice(GLuint targetFbo, GLuint textureToView) {
        glBindFramebuffer(GL_FRAMEBUFFER, targetFbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        debugSliceMaterial->SetTexture3D("_Volume", textureToView);
        debugSliceMaterial->SetFloat("_SliceZ", static_cast<float>(sliceZ) / static_cast<float>(resolution.z));
        debugSliceMaterial->SetInt("_Mode", mode);
        debugSliceMaterial->SetFloat("_DisplayScale", displayScale);
        debugSliceMaterial->Bind();
        quadModel->render();

        glEnable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Raymarcher::SetWorldBounds(glm::vec3 min, glm::vec3 max) {
        worldMin = min;
        worldMax = max;
        MarkDirty();
    }

} // core