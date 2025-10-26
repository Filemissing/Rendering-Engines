//
// Created by micha on 13/10/2025.
//

#ifndef RAWENGINE_CAMERA_H
#define RAWENGINE_CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Component.h"

namespace core {
    class Camera : public Component {
    private:
        int width, height;

    public:
        glm::mat4 getView();
        glm::mat4 projection;

        Camera(int width, int height);
    };
}

#endif //RAWENGINE_CAMERA_H