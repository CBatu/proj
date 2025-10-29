#pragma once
#include "gfx/shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <glad/glad.h>

struct Color {
    float r, g, b, a;
};

struct DrawCall {
    glm::mat4 model;
    Color color;
};

class Renderer3D {
public:
    bool Init();
    void Begin(const glm::mat4& viewProjMatrix);
    void DrawCube(const glm::mat4& model, Color color);
    void End();
    void Destroy();

private:
    void SetupCube();
    void FlushBatch();

    Shader shader;
    unsigned int VAO = 0, VBO = 0, instanceVBO = 0, colorVBO = 0;
    glm::mat4 viewProj;
    std::vector<DrawCall> drawCalls;
};