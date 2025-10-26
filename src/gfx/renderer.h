#pragma once
#include "gfx/shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>


struct Color {
    float r, g, b, a;
};

struct DrawCall {
    int baseVertex;        // vertexBuffer'daki başlangıç indeksi
    int vertexCount;       // Kaç vertex çizileceği (örn: Quad için 6, Triangle için 3)
    glm::mat4 model;
};


class Renderer {
public:
    bool Init();
    void Begin(const glm::mat4& viewProj);
    void End();

    void DrawTriangle(const glm::vec2& a,const glm::vec2& b,
                            const glm::vec2& c,
                            Color color,
                            const glm::mat4& model);

void DrawQuad(const glm::vec2& p1, const glm::vec2& p2,
                        const glm::vec2& p3, const glm::vec2& p4,
                        Color color,
                        const glm::mat4& model);

    void Destroy();

private:
    Shader shader;
    unsigned int VAO = 0, VBO = 0,instanceVBO = 0;
    glm::mat4 viewProj;
    std::vector<DrawCall> drawCalls;
    std::vector<glm::mat4> modelMatrices;
    std::map<unsigned int, std::vector<DrawCall>> batches; 
    
    std::vector<float> vertexBuffer;

};
