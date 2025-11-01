#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <string>

struct Material {
    GLuint diffuseTexture = 0;      // Diffuse / albedo map
    GLuint specularTexture = 0;     // Opsiyonel
    glm::vec3 color = {1.0f, 1.0f, 1.0f}; // Base color if no texture

    bool HasDiffuseTexture() const { return diffuseTexture != 0; }
};
