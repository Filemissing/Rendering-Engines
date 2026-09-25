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

        glGenBuffers(1, &primitivesBuffer);

        volumeTex = 0;

        marchMaterial = new Material(
            "Assets/shaders/PostProcessing/viewSpace.vert",
            "Assets/shaders/raymarching/march.frag");
        debugSliceMaterial = new Material(
            "Assets/shaders/PostProcessing/viewSpace.vert",
            "Assets/shaders/debug/slice_view.frag");

        noise2DShader = new ComputeShader("Assets/shaders/raymarching/noise2D.comp");
        noise3DShader = new ComputeShader("Assets/shaders/raymarching/noise3D.comp");

        EnsureNoiseResourcesSized();

        classifyShader = new ComputeShader("Assets/shaders/raymarching/classify.comp");
        jfaStepShader = new ComputeShader("Assets/shaders/raymarching/jfaStep.comp");
        jfaFinalizeShader = new ComputeShader("Assets/shaders/raymarching/jfaFinalize.comp");

        std::vector<Mesh> meshes = std::vector<Mesh>();
        meshes.push_back(Mesh::generateQuad());
        quadModel = new Model(std::move(meshes));

        EnsureVolumeSized(glm::ivec3(512));
        SetWorldBounds(glm::vec3(-30), glm::vec3(30));
    }
    Raymarcher::~Raymarcher() {
        glDeleteBuffers(1, &primitivesBuffer);
        DestroyFbo();
        DestroyNoiseResources();

        delete marchMaterial;
        delete quadModel;

        delete noise2DShader;
        delete noise3DShader;

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
    void Raymarcher::EnsureNoiseResourcesSized() {
        DestroyNoiseResources();

        // 2D noise
        glGenTextures(1, &noise2DTexture);
        glBindTexture(GL_TEXTURE_2D, noise2DTexture);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_R16F,
            noise2DResolution.x,
            noise2DResolution.y,
            0,
            GL_RED,
            GL_HALF_FLOAT,
            nullptr
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


        // 3D noise
        glGenTextures(1, &noise3DTexture);
        glBindTexture(GL_TEXTURE_3D, noise3DTexture);

        glTexImage3D(
            GL_TEXTURE_3D,
            0,
            GL_R16F,
            noise3DResolution.x,
            noise3DResolution.y,
            noise3DResolution.z,
            0,
            GL_RED,
            GL_HALF_FLOAT,
            nullptr
        );

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindTexture(GL_TEXTURE_3D, 0);
    }
    void Raymarcher::DestroyFbo() {
        if (volumeTex) glDeleteTextures(1, &volumeTex);
        if (jfaTexA) glDeleteTextures(1, &jfaTexA);
        if (jfaTexB) glDeleteTextures(1, &jfaTexB);
        if (signTex) glDeleteTextures(1, &signTex);
    }
    void Raymarcher::DestroyNoiseResources()
    {
        if (noise2DTexture) {
            glDeleteTextures(1, &noise2DTexture);
            noise2DTexture = 0;
        }

        if (noise3DTexture) {
            glDeleteTextures(1, &noise3DTexture);
            noise3DTexture = 0;
        }
    }

    void Raymarcher::UploadPrimitives(GLuint shader, const std::vector<Primitive*>& primitives) const {
        glUseProgram(shader);

        // assert offsets
        static_assert(offsetof(GPUPrimitive, position) == 0);
        static_assert(offsetof(GPUPrimitive, rotation) == 16);
        static_assert(offsetof(GPUPrimitive, scale)    == 32);
        static_assert(offsetof(GPUPrimitive, data)     == 48);
        static_assert(offsetof(GPUPrimitive, color)    == 64);
        static_assert(sizeof(GPUPrimitive) == 80);

        auto* GPUPrimitives = new GPUPrimitive[primitives.size()];

        for (size_t i = 0; i < primitives.size(); i++) {
            GPUPrimitives[i] = primitives[i]->GetGPUPrimitive();
        }

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, primitivesBuffer);

        glBufferData(
            GL_SHADER_STORAGE_BUFFER,
            primitives.size() * sizeof(GPUPrimitive),
            GPUPrimitives,
            GL_DYNAMIC_DRAW
        );
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, primitivesBuffer);

        GLint loc = glGetUniformLocation(shader, "primitiveCount");
        if (loc != -1) {
            glUniform1i(loc, static_cast<GLint>(primitives.size()));
        }

        delete[] GPUPrimitives;
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
        classifyShader->SetTexture2D("_Noise2D", noise2DTexture);
        classifyShader->SetTexture3D("_Noise3D", noise3DTexture);
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

    void Raymarcher::BakeNoise() {
        EnsureNoiseResourcesSized();

        // 2D noise
        noise2DShader->SetVec3("_Resolution", glm::vec3(noise2DResolution, 0));
        noise2DShader->SetInt("_Seed", noiseSeed);
        noise2DShader->SetFloat("_Frequency", noise2DFrequency);

        noise2DShader->Bind();

        noise2DShader->BindImage(
            0,
            noise2DTexture,
            GL_WRITE_ONLY,
            GL_R16F
        );

        const int noise2DGX = (noise2DResolution.x + 7) / 8;
        const int noise2DGY = (noise2DResolution.y + 7) / 8;

        noise2DShader->Dispatch(noise2DGX,noise2DGY,1);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

        // 3D noise
        noise3DShader->SetVec3("_Resolution", glm::vec3(noise3DResolution));
        noise3DShader->SetInt("_Seed", noiseSeed);
        noise3DShader->SetFloat("_Frequency", noise3DFrequency);

        noise3DShader->Bind();

        noise3DShader->BindImage(
            0,
            noise3DTexture,
            GL_WRITE_ONLY,
            GL_R16F
        );

        const int noise3DGX = (noise3DResolution.x + 7) / 8;
        const int noise3DGY = (noise3DResolution.y + 7) / 8;
        const int noise3DGZ = (noise3DResolution.z + 7) / 8;

        noise3DShader->Dispatch(noise3DGX,noise3DGY,noise3DGZ);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
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
            if (debugTex == dNoise3DTex) dTex = noise3DTexture;
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