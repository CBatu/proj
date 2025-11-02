#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <stb_image.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

struct Material {
    GLuint diffuseTexture = 0; // 0 ise texture yok
    glm::vec3 color = {1.0f, 1.0f, 1.0f};
    float shininess = 32.0f;
    
    bool HasTexture() const { return diffuseTexture != 0; }
    bool LoadTexture(const std::string& path) {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true); // opsiyonel: OpenGL ile uyumlu
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
        if (!data) {
            std::cerr << "Failed to load texture: " << path << std::endl;
            // Print current working directory to help debugging resource paths
#if defined(_WIN32)
            char buf[512];
            if (GetCurrentDirectoryA(sizeof(buf), buf)) std::cerr << "CWD: " << buf << std::endl;
#else
            char buf[PATH_MAX];
            if (getcwd(buf, sizeof(buf))) std::cerr << "CWD: " << buf << std::endl;
#endif
            return false;
        }

        glGenTextures(1, &diffuseTexture);
        glBindTexture(GL_TEXTURE_2D, diffuseTexture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Basit filtering & wrap
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        return true;
    }
};
