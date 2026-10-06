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
        GLuint m_timeQuery = 0;
        bool m_queryInFlight = false;

        GLuint primitivesBuffer = 0;

        // general settings
        glm::ivec3 resolution{};
        glm::vec3 worldMin{}, worldMax{};
        bool dirty = true;

        // noise generation
        GLuint noise2DTex = 0;
        GLuint noise3DTex = 0;

        ComputeShader* noise2DShader;
        ComputeShader* noise3DShader;

        // JFA
        ComputeShader* classifyShader = nullptr;
        ComputeShader* jfaStepShader = nullptr;
        ComputeShader* jfaFinalizeShader = nullptr;
        // seed-position ping-pong buffers: xyz = nearest seed voxel coord, w = valid flag
        GLuint jfaTexA = 0, jfaTexB = 0;
        GLuint finalSeedTex = 0;
        GLuint signTex = 0;

        void EnsureJFAResourcesSized();
        void DestroyJFAResources();

        // brick pool
        static constexpr int BRICK_CORE = 8;
        static constexpr int BRICK_STORE = BRICK_CORE + 1; // 9 — includes the apron sample

        glm::ivec3 brickGridResolution = glm::ivec3(0); // number of bricks per axis
        glm::ivec3 brickPoolDim = glm::ivec3(32); // the dimensions of the brick pool

        GLuint allocationCounter = 0; // atomic counter buffer for brick allocation

        GLuint brickPoolTex = 0;   // R16F, holds baked distances, laid out as brickGridResolution * BRICK_STORE texels
        GLuint brickActiveTex = 0;
        GLuint lookupTex = 0; // RGBA32F, one texel per brick, xyz = this brick's own origin in pool space, w reserved
        GLuint reverseLookupTex = 0; // RGBA32F, sized brickPoolDim, points to the bricks world index

        ComputeShader* clearShader;
        ComputeShader* allocateShader;

        void EnsureBrickResourcesSized();
        void DestroyBrickResources();

        // final output
        Material* marchMaterial;
        Material* debugSliceMaterial;
        Material* debugTextureMaterial;
        Model* quadModel;

        void UploadPrimitives(GLuint shader, const std::vector<Primitive*>& primitives) const;

    public:
        // general settings
        int maxSteps = 256;
        float maxDist = 200.0f;
        float surfDist = 0.001f;

        // noise settings
        glm::ivec2 noise2DResolution{512, 512};
        glm::ivec3 noise3DResolution{256, 256, 256};

        float noise2DFrequency = 1.0f;
        float noise3DFrequency = 1.0f;

        unsigned int noiseSeed = 12345;

        void EnsureNoiseResourcesSized();
        void DestroyNoiseResources();
        void BakeNoise();

        // terrain settings
        int octaves = 8;
        float warpStrength = 1.0f;
        float lacunarity = 2.0f;
        float persistence = 0.3f;
        float heightScale = 8.0f;

        // debug settings
        bool debug3D = false;
        bool debug2D = false;
        int sliceZ = 0;
        int mode = 0;
        float displayScale = 1.0f;
        enum DebugTexture3D {
            dSignTex,
            dSeedTex,
            dNoise3DTex,
            dBrickActiveTex,
            dLookupTex,
            dReverseLookupTex,
            dBrickPoolTex
        };
        DebugTexture3D debugTex3D = dSignTex;
        enum DebugTexture2D {
            dNoise2DTex
        };
        DebugTexture2D debugTex2D = dNoise2DTex;

        Raymarcher();
        ~Raymarcher();

        // general methods
        void EnsureVolumeSized(glm::ivec3 newResolution);
        void Bake(const std::vector<Primitive*>& primitives);
        void Render(GLuint targetFbo, GLuint sceneColorTex, GLuint sceneDepthTex, Camera* cam);
        void RenderDebugSlice(GLuint targetFbo, GLuint textureToView);
        void RenderDebugTexture(GLuint targetFbo, GLuint textureToView);

        void SetWorldBounds(glm::vec3 min, glm::vec3 max);
        void MarkDirty() {dirty = true;}
        bool IsDirty() const { return dirty; }

        glm::ivec3 GetResolution() const { return resolution; }

        glm::ivec3 GetBrickPoolDim() const { return brickPoolDim; }
        void SetBrickPoolDim(const glm::ivec3 dim) { brickPoolDim = dim; }
    };
} // core

#endif //RAWENGINE_RAYMARCHER_H