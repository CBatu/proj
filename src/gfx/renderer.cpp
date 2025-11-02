#include "gfx/renderer.h"
#include <stb_image.h>
#include <iostream>
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
// Optional. define TINYOBJLOADER_USE_MAPBOX_EARCUT gives robust triangulation. Requires C++11
//#define TINYOBJLOADER_USE_MAPBOX_EARCUT
#include "tiny_obj_loader.h"

bool Renderer3D::Init() {
    if (!shader.Load("shaders/basic.vert", "shaders/basic.frag")) {
        std::cerr << "Shader load failed!\n";
        return false;
    }
    SetupCube();
    // ensure sampler uses texture unit 0
    shader.Use();
    shader.SetInt("uDiffuseTex", 0);

    glEnable(GL_DEPTH_TEST);
    return true;
}

void Renderer3D::SetupCube() {
    // Interleaved vertex data: position (3), texcoord (2)
    float cubeVertices[] = {
        // back face (z = -0.5)
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        // front face (z = 0.5)
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        // left face (x = -0.5)
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        // right face (x = 0.5)
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        // bottom face (y = -0.5)
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        // top face (y = 0.5)
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    const size_t MAX_INSTANCES = 1024; // ihtiyaca göre arttır

    // generate VAO/VBOs
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &instanceVBO);
    glGenBuffers(1, &colorVBO);

    glBindVertexArray(VAO);

    // vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // positions (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // texcoords (location = 6)
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    // instance matrices (locations 1,2,3,4)
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCES * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(1 + i);
        glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
        glVertexAttribDivisor(1 + i, 1);
    }

    // instance colors (location = 5)
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCES * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glVertexAttribDivisor(5, 1);

    // cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void Renderer3D::Begin(const glm::mat4& viewProjMatrix, const glm::mat4& cameraPos) {
    viewProj = viewProjMatrix;
    drawCalls.clear();
    lights.clear();

    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    

    shader.Use();
    shader.SetMat4("uViewProj", viewProj);


    // Kamera pozisyonu (specular eklersek lazım olacak)
    shader.SetVec3("uViewPos", glm::vec3(cameraPos[3])); 
}


void Renderer3D::UploadLights() {
    //shader.Use();
    shader.SetInt("uLightCount", (int)lights.size());
    for (size_t i = 0; i < lights.size(); i++) {
        std::string prefix = "uLights[" + std::to_string(i) + "]";
        shader.SetInt(prefix + ".type", lights[i].type);
        shader.SetVec3(prefix + ".position", lights[i].position);
        shader.SetVec3(prefix + ".direction", lights[i].direction);
        shader.SetVec3(prefix + ".color", glm::vec3(lights[i].color.r, lights[i].color.g, lights[i].color.b));
        shader.SetFloat(prefix + ".intensity", lights[i].intensity);
    }
}


void Renderer3D::DrawCube(const glm::mat4& model, Material* mat) {
    drawCalls.push_back({ model, mat });
}

void Renderer3D::FlushBatch() {
    if (drawCalls.empty()) return;

    shader.Use();
    glBindVertexArray(VAO);

    // --- Batchleri ayır: texture’lı ve texturesiz ---
    std::vector<glm::mat4> matricesTex, matricesNoTex;
    std::vector<glm::vec4> colorsTex, colorsNoTex;
    GLuint boundTexture = 0;

    for (auto& call : drawCalls) {
        Material* mat = call.material;
        if (mat && mat->HasTexture()) {
            matricesTex.push_back(call.model);
            colorsTex.push_back(glm::vec4(mat->color, 1.0f));
            boundTexture = mat->diffuseTexture; // son kullanılan texture
        } else {
            matricesNoTex.push_back(call.model);
            glm::vec3 color = mat ? mat->color : glm::vec3(1.0f);
            colorsNoTex.push_back(glm::vec4(color, 1.0f));
        }
    }

    // === TEXTURE’LI OBJELER ===
    if (!matricesTex.empty() && boundTexture != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, boundTexture);
        shader.SetInt("uUseTexture", 1);

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, matricesTex.size() * sizeof(glm::mat4), matricesTex.data());
        glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, colorsTex.size() * sizeof(glm::vec4), colorsTex.data());

        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, (GLsizei)matricesTex.size());
    }

    // === SADECE RENKLİ OBJELER ===
    if (!matricesNoTex.empty()) {
        shader.SetInt("uUseTexture", 0);

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, matricesNoTex.size() * sizeof(glm::mat4), matricesNoTex.data());
        glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, colorsNoTex.size() * sizeof(glm::vec4), colorsNoTex.data());

        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, (GLsizei)matricesNoTex.size());
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}



void Renderer3D::End() {
    FlushBatch();
}

void Renderer3D::Destroy() {
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &instanceVBO);
    glDeleteBuffers(1, &colorVBO);
    glDeleteVertexArrays(1, &VAO);
    shader.Destroy();
}