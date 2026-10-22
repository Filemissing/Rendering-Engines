#pragma once

#include <glad/glad.h>
#include <string>

#include "../Asset.h"

namespace core {

    class Texture : public Asset {
    private:
        GLuint id;

    public:
        Texture(const std::string& path);

        GLuint getId();
    };

}
