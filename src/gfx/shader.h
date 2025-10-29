#pragma once
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader {
public:
    unsigned int ID = 0;
    bool Load(const std::string& vertPath, const std::string& fragPath);
    void Use() const { glUseProgram(ID); }
    void Destroy();
    void SetMat4(const std::string& name, const glm::mat4& m) {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &m[0][0]);
    }

    void SetVec4(const std::string& name, const glm::vec4& v) {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), v.x, v.y, v.z, v.w);
    }

private:
    std::string ReadFile(const std::string& path);
    unsigned int Compile(unsigned int type, const std::string& source);
};
