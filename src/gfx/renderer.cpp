#include "gfx/renderer.h"
#include <iostream>

constexpr unsigned int SHAPE_TRIANGLE = 0; // (ShapeType::Triangle'ın int karşılığı)
constexpr unsigned int SHAPE_QUAD = 1;

bool Renderer::Init() {
    if (!shader.Load("shaders/basic.vert", "shaders/basic.frag")) {
            std::cerr << "Shader load failed!\n";
            return false;
        }

        // VAO & VBO setup
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &instanceVBO);

        glBindVertexArray(VAO);

        // Vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // boş buffer, per-frame güncellenecek
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);

        // Instance buffer
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        for (int i = 0; i < 4; i++) {
            glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float)*i*4));
            glEnableVertexAttribArray(2 + i);
            glVertexAttribDivisor(2 + i, 1); // 1 matris per instance
        }

        return true;
}

void Renderer::Begin(const glm::mat4& viewProjMatrix) {
    viewProj = viewProjMatrix;
    vertexBuffer.clear();
}

void Renderer::End() {

if (vertexBuffer.empty() || modelMatrices.empty()) return;

        shader.Use();

        // Vertex buffer
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), vertexBuffer.data(), GL_DYNAMIC_DRAW);

        // Instance buffer
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), modelMatrices.data(), GL_DYNAMIC_DRAW);

        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "uViewProj"), 1, GL_FALSE, &viewProj[0][0]);

        int vertexCount = vertexBuffer.size() / 7;
        glDrawArraysInstanced(GL_TRIANGLES, 0, vertexCount, modelMatrices.size());

}


void Renderer::DrawTriangle(const glm::vec2& a,
                            const glm::vec2& b,
                            const glm::vec2& c,
                            Color color,
                            const glm::mat4& model)
{
    // Local space vertexleri
    float verts[] = {
        a.x, a.y, 0.0f, color.r, color.g, color.b, color.a,
        b.x, b.y, 0.0f, color.r, color.g, color.b, color.a,
        c.x, c.y, 0.0f, color.r, color.g, color.b, color.a
    };

    vertexBuffer.insert(vertexBuffer.end(), std::begin(verts), std::end(verts));
    modelMatrices.push_back(model);
}


void Renderer::DrawQuad(const glm::vec2& p1, const glm::vec2& p2,
                        const glm::vec2& p3, const glm::vec2& p4,
                        Color color,
                        const glm::mat4& model)
{
float verts[] = {
            p1.x, p1.y, 0.0f, color.r, color.g, color.b, color.a,
            p2.x, p2.y, 0.0f, color.r, color.g, color.b, color.a,
            p3.x, p3.y, 0.0f, color.r, color.g, color.b, color.a,

            p1.x, p1.y, 0.0f, color.r, color.g, color.b, color.a,
            p3.x, p3.y, 0.0f, color.r, color.g, color.b, color.a,
            p4.x, p4.y, 0.0f, color.r, color.g, color.b, color.a
        };

        vertexBuffer.insert(vertexBuffer.end(), std::begin(verts), std::end(verts));
        modelMatrices.push_back(model);
}


void Renderer::Destroy() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &instanceVBO);
    glDeleteBuffers(1, &VBO);
    shader.Destroy();
}
