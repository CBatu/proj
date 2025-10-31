#include "gfx/renderer.h"
#include <iostream>

bool Renderer3D::Init() {
    if (!shader.Load("shaders/basic.vert", "shaders/basic.frag")) {
        std::cerr << "Shader load failed!\n";
        return false;
    }
    SetupCube();
    glEnable(GL_DEPTH_TEST);
    return true;
}

void Renderer3D::SetupCube() {
    float cubeVertices[] = {
        -0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
        -0.5f,  0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f,  

        -0.5f, -0.5f,  0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f,  

        -0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f,  
        -0.5f, -0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f,  

         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  

        -0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f, -0.5f,  

        -0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f, -0.5f
    };
const size_t MAX_INSTANCES = 1024; // ihtiyaca göre arttır

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &instanceVBO);
    glGenBuffers(1, &colorVBO);

    glBindVertexArray(VAO);

    // --- static vertex positions ---
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); // layout(location = 0) aPos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // --- instance matrices: allocate GPU storage once ---
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCES * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);

    std::size_t vec4Size = sizeof(glm::vec4);
    // mat4 -> 4 vec4 attribute (locations 1,2,3,4)
    for (int i = 0; i < 4; ++i) {
        glEnableVertexAttribArray(1 + i);
        glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * vec4Size));
        glVertexAttribDivisor(1 + i, 1);
    }

    // --- instance colors: allocate ---
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCES * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(5); // layout(location = 5) aColor
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glVertexAttribDivisor(5, 1);

    // unbind for cleanliness
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


void Renderer3D::DrawCube(const glm::mat4& model, Color color) {
    drawCalls.push_back({ model, color });
}

void Renderer3D::FlushBatch() {
    if (drawCalls.empty()) return;

    shader.Use();
    glBindVertexArray(VAO);

    // CPU-side hazırlık
    std::vector<glm::mat4> matrices(drawCalls.size());
    std::vector<glm::vec4> colors(drawCalls.size());

    for (size_t i = 0; i < drawCalls.size(); ++i) {
        matrices[i] = drawCalls[i].model;
        colors[i] = glm::vec4(drawCalls[i].color.r, drawCalls[i].color.g, drawCalls[i].color.b, drawCalls[i].color.a);
    }

    // === safety checks ===
    if (!matrices.empty()) {
        // bind the correct buffer BEFORE subdata
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        // If you might exceed MAX_INSTANCES you should reallocate:
        // glBufferData(GL_ARRAY_BUFFER, newSize, nullptr, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, matrices.size() * sizeof(glm::mat4), matrices.data());
    }

    if (!colors.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, colors.size() * sizeof(glm::vec4), colors.data());
    }

    // ensure viewproj uniform
    shader.SetMat4("uViewProj", viewProj);

    // draw
    GLsizei instanceCount = static_cast<GLsizei>(drawCalls.size());
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, instanceCount);

    // cleanup
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