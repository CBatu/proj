#pragma once
#include <string>
#include <glad/glad.h>

class Shader {
public:
    unsigned int ID = 0;
    bool Load(const std::string& vertPath, const std::string& fragPath);
    void Use() const { glUseProgram(ID); }
    void Destroy();

private:
    std::string ReadFile(const std::string& path);
    unsigned int Compile(unsigned int type, const std::string& source);
};
