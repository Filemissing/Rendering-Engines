//
// Created by micha on 02/12/2025.
//

#ifndef RAWENGINE_RENDERER_H
#define RAWENGINE_RENDERER_H
#include "assimpLoader.h"
#include "Assets/Material.h"
#include "Assets/model.h"
#include "Assets/Scene.h"
#include "PostProcessing.h"
#include "Raymarcher.h"
#include "RaymarchVersions/GpuTimer.h"
#include "RaymarchVersions/RaymarchBackend.h"

namespace core {
    class Renderer {
        GLuint m_timeQuery = 0;
        bool m_queryInFlight = false;

        GLuint sceneFbo;
        GLuint sceneColor;
        GLuint sceneDepth;

        GLuint ppFbo[2];
        GLuint ppTex[2];
        GLuint ppDepth[2];

        Model* quadModel;

        std::unique_ptr<RaymarchBackend> m_backend;
        std::string m_backendId;

    public:
        // GPU time of the raymarch pass only; recorded under this tag while it is non-empty
        GpuTimer raymarchTimer;
        std::string raymarchTimerTag;
        // increases once per RenderScene; the viewport skips rendering while it is not visible
        uint64_t renderedFrames = 0;

        Renderer();
        ~Renderer();

        std::vector<PostProcessEffect*> postProcessingEffects = std::vector<PostProcessEffect*>();
        void RenderScene(Scene*& scene, const GLuint& finalFbo, const GLuint& finalTexture, int width, int height);
        void EnsureFboSized(int width, int height);
        void DestroyFbo();

        // id from GetRaymarchVersions(), or "" for none
        bool SetRaymarchVersion(const std::string& id);
        const std::string& GetRaymarchVersion() const { return m_backendId; }
        RaymarchBackend* GetBackend() const { return m_backend.get(); }

        // the current-version raymarcher; nullptr while an older version is selected
        Raymarcher* GetRaymarcher() const { return m_backend ? m_backend->GetCurrentRaymarcher() : nullptr; }
    };
} // core

#endif //RAWENGINE_RENDERER_H