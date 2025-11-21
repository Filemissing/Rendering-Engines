#pragma once

#include <vector>
#include <glad/glad.h>
#include "../vertex.h"

namespace core {
    class Mesh {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        GLuint VAO;
        GLuint VBO;
        GLuint EBO;
    public:
        Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices);
        ~Mesh();

        Mesh(Mesh&& other) noexcept;
        Mesh& operator=(Mesh&& other) noexcept;

        void render();
        static Mesh generateQuad();
    private:
        void setupBuffers();
    };
}