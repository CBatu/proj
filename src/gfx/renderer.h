#pragma once
#include "gfx/shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Node/Material.h"
#include <vector>
#include <glad/glad.h>

struct Color {
    float r, g, b, a;
};

struct DrawCall {
    glm::mat4 model;
    Material* material = nullptr; // nullptr ise sadece renk kullan
};


struct LightData {
    int type; // 0 = directional, 1 = point
    glm::vec3 direction;
    glm::vec3 position;
    Color color;
    float intensity;
    float radius;
};

class Renderer3D {
public:
    bool Init();
    void Begin(const glm::mat4& viewProjMatrix, const glm::mat4& cameraPos);
    void DrawCube(const glm::mat4& model,Material* material);
    void End();
    void Destroy();
    void AddLight(const LightData& light) {
        lights.push_back(light);
    }
    void UploadLights();

private:
    void SetupCube();
    void FlushBatch();

    Shader shader;
    unsigned int VAO = 0, VBO = 0, instanceVBO = 0, colorVBO = 0;
    glm::mat4 viewProj;
    std::vector<DrawCall> drawCalls;
    std::vector<LightData> lights;
};