//
// Created by micha on 13/09/2026.
//

#include "ComputeShader.h"

#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.inl>

namespace core {
    static std::string ReadFileToString(const std::string& filePath) {
        std::ifstream fileStream(filePath, std::ios::in);
        if (!fileStream.is_open()) {
            printf("Could not open file: %s\n", filePath.c_str());
            return "";
        }
        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        return buffer.str();
    }

    ComputeShader::ComputeShader(const std::string& computeShaderPath) {
        const std::string shaderText = ReadFileToString(computeShaderPath);
        const char* src = shaderText.c_str();

        GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            printf("Error! Compute shader issue [%s]: %s\n", computeShaderPath.c_str(), infoLog);
        }

        program = glCreateProgram();
        glAttachShader(program, shader);
        glLinkProgram(program);

        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(program, 512, nullptr, infoLog);
            printf("Error! Linking compute shader program: %s\n", infoLog);
        }

        glDeleteShader(shader);
    }

    ComputeShader::~ComputeShader() {
        glDeleteProgram(program);
    }

    void ComputeShader::Bind() const {
        glUseProgram(program);

        // upload uniforms
        for (auto& [name, value] : vec3Uniforms) {
            GLint loc = glGetUniformLocation(program, name.c_str());
            if (loc == -1) continue; // uniform not found
            glUniform3fv(loc, 1, glm::value_ptr(value));
        }
        for (auto& [name, value] : vec4Uniforms) {
            GLint loc = glGetUniformLocation(program, name.c_str());
            if (loc == -1) continue; // uniform not found
            glUniform4fv(loc, 1, glm::value_ptr(value));
        }
        for (auto& [name, value] : mat4Uniforms) {
            GLint loc = glGetUniformLocation(program, name.c_str());
            if (loc == -1) continue; // uniform not found
            glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
        }
        for (auto& [name, value] : floatUniforms) {
            GLint loc = glGetUniformLocation(program, name.c_str());
            if (loc == -1) continue; // uniform not found
            glUniform1f(loc, value);
        }
        for (auto& [name, value] : intUniforms) {
            GLint loc = glGetUniformLocation(program, name.c_str());
            if (loc == -1) continue;
            glUniform1i(loc, value);
        }

        // bind textures
        int i = 0;
        for (auto& [name, tex] : textures2D) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, tex);
            glUniform1i(glGetUniformLocation(program, name.c_str()), i);
            i++;
        }
        for (auto& [name, tex] : textures3D) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_3D, tex);
            glUniform1i(glGetUniformLocation(program, name.c_str()), i);
            i++;
        }
    }

    void ComputeShader::SetTexture2D(const std::string& name, const GLuint tex) {
        textures2D[name] = tex;
    }
    void ComputeShader::SetTexture3D(const std::string& name, const GLuint tex) {
        textures3D[name] = tex;
    }
    void ComputeShader::SetVec3(const std::string& name, const glm::vec3& value) {
        vec3Uniforms[name] = value;
    }
    void ComputeShader::SetVec4(const std::string& name, const glm::vec4& value) {
        vec4Uniforms[name] = value;
    }
    void ComputeShader::SetMat4(const std::string& name, const glm::mat4& value) {
        mat4Uniforms[name] = value;
    }
    void ComputeShader::SetFloat(const std::string& name, float value) {
        floatUniforms[name] = value;
    }
    void ComputeShader::SetInt(const std::string& name, int value) {
        intUniforms[name] = value;
    }

    void ComputeShader::BindImage(const GLuint unit, const GLuint texture, const GLenum access, const GLenum format) {
        // level=0, layered=GL_TRUE (bind the whole 3D texture, not one slice), layer=0 ignored when layered
        glBindImageTexture(unit, texture, 0, GL_TRUE, 0, access, format);
    }

    void ComputeShader::Dispatch(const int groupsX, const int groupsY, const int groupsZ) {
        glDispatchCompute(static_cast<GLuint>(groupsX), static_cast<GLuint>(groupsY), static_cast<GLuint>(groupsZ));
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
} // core