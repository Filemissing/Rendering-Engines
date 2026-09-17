//
// Created by micha on 09/09/2026.
//

#ifndef RAWENGINE_RAYMARCHER_H
#define RAWENGINE_RAYMARCHER_H
#include "Assets/ComputeShader.h"
#include "Assets/Material.h"
#include "Assets/model.h"
#include "Components/Camera.h"
#include "Components/Primitive.h"

namespace core {
    class Raymarcher {
        GLuint volumeTex;
        Material* marchMaterial;
        Material* debugSliceMaterial;

        Model* quadModel;

        // JFA
        ComputeShader* classifyShader = nullptr;
        ComputeShader* jfaStepShader = nullptr;
        ComputeShader* jfaFinalizeShader = nullptr;
        // seed-position ping-pong buffers: xyz = nearest seed voxel coord, w = valid flag (0/1)
        GLuint jfaTexA = 0, jfaTexB = 0;
        GLuint finalSeedTex = 0;
        GLuint signTex = 0;

        void EnsureJFAResourcesSized();

        // general settings
        glm::ivec3 resolution;
        glm::vec3 worldMin, worldMax;
        bool dirty = true;

    public:
        // general settings
        int maxSteps = 96;
        float maxDist = 200.0f;
        float surfDist = 0.001f;

        // terrain settings
        int octaves = 8;
        float warpStrength = 1.0f;
        float lacunarity = 2.0f;
        float persistence = 0.3f;
        float heightScale = 8.0f;

        // debug settings
        bool debug = false;
        int sliceZ = 0;
        int mode = 0;
        float displayScale = 1.0f;
        enum DebugTexture {
            dSignTex,
            dSeedTex,
            dVolumeTex
        };
        DebugTexture debugTex = dSignTex;

        Raymarcher();
        ~Raymarcher();

        void EnsureVolumeSized(glm::ivec3 newResolution);
        void DestroyFbo();
        void Bake(const std::vector<Primitive*>& primitives);
        void Render(GLuint targetFbo, GLuint sceneColorTex, GLuint sceneDepthTex, Camera* cam);
        void RenderDebugSlice(GLuint targetFbo, GLuint textureToView);

        void SetWorldBounds(glm::vec3 min, glm::vec3 max);
        void MarkDirty() {dirty = true;}
        bool IsDirty() const { return dirty; }

        glm::ivec3 GetResolution() const { return resolution; }
        GLuint GetVolumeTexture() const { return volumeTex; }
    };
} // core

#endif //RAWENGINE_RAYMARCHER_H