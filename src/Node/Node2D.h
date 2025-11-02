#pragma once
#include "gfx/renderer.h"
#include <glm/glm.hpp>
#include "Node/Material.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Node3D {
public:
    glm::vec3 position {0.0f};
    glm::vec3 rotation {0.0f}; // Euler angles (radians)
    glm::vec3 scale {1.0f};

    Node3D* parent = nullptr;
    std::vector<Node3D*> children;

    virtual ~Node3D() {}

    void AddChild(Node3D* child) {
        child->parent = this;
        children.push_back(child);
    }

    glm::mat4 GetTransform() const {
        glm::mat4 model(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, rotation.x, glm::vec3(1,0,0));
        model = glm::rotate(model, rotation.y, glm::vec3(0,1,0));
        model = glm::rotate(model, rotation.z, glm::vec3(0,0,1));
        model = glm::scale(model, scale);
        return model;
    }

    glm::mat4 GetGlobalTransform() const {
        if (parent)
            return parent->GetGlobalTransform() * GetTransform();
        return GetTransform();
    }

    virtual void Update(float dt) {
        for (auto c : children)
            c->Update(dt);
    }

    virtual void Render(class Renderer3D& renderer, const glm::mat4& viewProj) {
        for (auto c : children)
            c->Render(renderer, viewProj);
    }
};

class MeshNode3D : public Node3D {
public:
    Material* material = nullptr;

    void Render(Renderer3D& renderer, const glm::mat4& viewProj) override {
        renderer.DrawCube(GetGlobalTransform(), material);
        Node3D::Render(renderer, viewProj);
    }
};


class CameraNode3D : public Node3D {
public:
    float fov = 60.0f;
    float aspect = 16.0f / 9.0f;
    float nearClip = 0.1f;
    float farClip = 100.0f;

    // FPS kamera için Euler açıları
    float yaw = -90.0f;   // sağa sola
    float pitch = 0.0f;   // yukarı aşağı
    glm::vec3 front = { 0.0f, 0.0f, -1.0f };
    glm::vec3 up = { 0.0f, 1.0f, 0.0f };
    glm::vec3 right = { 1.0f, 0.0f, 0.0f };

    void SetPerspective(float fovDeg, float aspectRatio, float nearZ, float farZ) {
        fov = fovDeg;
        aspect = aspectRatio;
        nearClip = nearZ;
        farClip = farZ;
    }

    // === FPS Look yönü hesapla ===
    void UpdateDirection() {
        glm::vec3 dir;
        dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        dir.y = sin(glm::radians(pitch));
        dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(dir);

        // Sağ ve yukarı vektörleri güncelle
        right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
        up    = glm::normalize(glm::cross(right, front));
    }

    glm::mat4 GetView() const {
        return glm::lookAt(position, position + front, up);
    }

    glm::mat4 GetProjection() const {
        return glm::perspective(glm::radians(fov), aspect, nearClip, farClip);
    }

    glm::mat4 GetViewProj() const {
        return GetProjection() * GetView();
    }

    glm::vec3 GetForward() const { return front; }
    glm::vec3 GetUp() const { return up; }
    glm::vec3 GetRight() const { return right; }
};


class LightNode3D : public Node3D {
public:
    Color color = {1.0f, 1.0f, 1.0f};
    float intensity = 1.0f;

    // Tipler: directional, point, spot
    enum class Type { Point, Directional, Spot } type = Type::Point;

    LightNode3D() = default;

    glm::vec3 GetWorldPosition() const {
        glm::mat4 transform = GetGlobalTransform();
        return glm::vec3(transform[3]);
    }

    glm::vec3 GetDirection() const {
        // Directional veya spotlight için
        glm::mat4 transform = GetGlobalTransform();
        glm::vec3 forward = {0,0,-1};
        return glm::normalize(glm::vec3(transform * glm::vec4(forward,0.0f)));
    }

    void Render(Renderer3D& renderer, const glm::mat4& viewProj) override {

        renderer.AddLight(LightData{
            type == Type::Directional ? 0 : 1,
            GetDirection(),
            GetWorldPosition(),
            color,
            intensity,
            0.0f // radius, gerekirse eklenebilir
        });
        Node3D::Render(renderer, viewProj);
    }
};
