#pragma once
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>

class Shader {
public:
    unsigned int ID = 0;

    bool Load(const std::string& vertPath, const std::string& fragPath);
    void Use() const { glUseProgram(ID); }
    void Destroy();

    // ==== Uniform Set Fonksiyonları ====

    void SetMat4(const std::string& name, const glm::mat4& m) const {
        GLint loc = glGetUniformLocation(ID, name.c_str());
        if (loc != -1)
            glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
    }

    void SetVec4(const std::string& name, const glm::vec4& v) const {
        GLint loc = glGetUniformLocation(ID, name.c_str());
        if (loc != -1)
            glUniform4f(loc, v.x, v.y, v.z, v.w);
    }

    void SetVec3(const std::string& name, const glm::vec3& v) const {
        GLint loc = glGetUniformLocation(ID, name.c_str());
        if (loc != -1)
            glUniform3f(loc, v.x, v.y, v.z);
    }

    void SetFloat(const std::string& name, float value) const {
        GLint loc = glGetUniformLocation(ID, name.c_str());
        if (loc != -1)
            glUniform1f(loc, value);
    }

    void SetInt(const std::string& name, int value) const {
        GLint loc = glGetUniformLocation(ID, name.c_str());
        if (loc != -1)
            glUniform1i(loc, value);
    }

private:
    std::string ReadFile(const std::string& path);
    unsigned int Compile(unsigned int type, const std::string& source);
};
