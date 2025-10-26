//
// Created by micha on 20/10/2025.
//

#ifndef RAWENGINE_RENDERSETTINGS_H
#define RAWENGINE_RENDERSETTINGS_H
#include <glad/glad.h>


class RenderSettings {
public:
    static void Init() {
        glEnable(GL_DEPTH_TEST);
        glFrontFace(GL_CCW);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    }
};


#endif //RAWENGINE_RENDERSETTINGS_H