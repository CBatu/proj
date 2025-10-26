#include "gfx/shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::string Shader::ReadFile(const std::string& path) {
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

unsigned int Shader::Compile(unsigned int type, const std::string& source) {
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(shader, 512, nullptr, info);
        std::cerr << "Shader compile error:\n" << info << std::endl;
    }
    return shader;
}

bool Shader::Load(const std::string& vertPath, const std::string& fragPath) {
    std::string vSrc = ReadFile(vertPath);
    std::string fSrc = ReadFile(fragPath);

    unsigned int vShader = Compile(GL_VERTEX_SHADER, vSrc);
    unsigned int fShader = Compile(GL_FRAGMENT_SHADER, fSrc);

    ID = glCreateProgram();
    glAttachShader(ID, vShader);
    glAttachShader(ID, fShader);
    glLinkProgram(ID);

    int success;
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        char info[512];
        glGetProgramInfoLog(ID, 512, nullptr, info);
        std::cerr << "Shader link error:\n" << info << std::endl;
        return false;
    }

    glDeleteShader(vShader);
    glDeleteShader(fShader);
    return true;
}

void Shader::Destroy() {
    if (ID)
        glDeleteProgram(ID);
    ID = 0;
}
