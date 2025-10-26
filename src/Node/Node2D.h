#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "gfx/renderer.h"

class Node2D {
public:
    glm::vec2 position {0.0f, 0.0f};
    glm::vec2 scale {1.0f, 1.0f};
    float rotation = 0.0f;
    glm::vec2 pivot {0.5f, 0.5f};
    bool visible = true;

    Node2D* parent = nullptr;
    std::vector<Node2D*> children;

    virtual ~Node2D() = default;

    // Transform hesaplanırken pivot'u merkeze al
    glm::mat4 GetTransform() const {
        glm::mat4 transform(1.0f);

        // 1. Pivotu merkeze al (örnek: 0.5f,0.5f -> ortası)
        transform = glm::translate(transform, glm::vec3(position, 0.0f));

        // Pivot ofsetini hesapla (local space'te)
        glm::vec3 pivotOffset = glm::vec3(-pivot, 0.0f);

        // 2. Pivot etrafında dönmesi için translate → rotate → scale → translate sırası
        transform = glm::translate(transform, -pivotOffset);
        transform = glm::rotate(transform, rotation, glm::vec3(0, 0, 1));
        transform = glm::scale(transform, glm::vec3(scale, 1.0f));
        transform = glm::translate(transform, pivotOffset);

        if (parent)
            return parent->GetTransform() * transform;
        return transform;
    }

    virtual void Update(float dt) {
        for (auto* child : children)
            child->Update(dt);
    }

    virtual void Draw(Renderer& renderer) {
        for (auto* child : children)
            child->Draw(renderer);
    }

    void AddChild(Node2D* child) {
        child->parent = this;
        children.push_back(child);
    }
};

enum class ShapeType {
    Triangle,
    Quad,
    Circle
};

class Shape2D : public Node2D {
public:
    ShapeType type = ShapeType::Quad;
    glm::vec2 size {100.0f, 100.0f};
    Color color {1.0f, 1.0f, 1.0f, 1.0f};

    void Draw(Renderer& renderer) override {
        if (!visible) return;

        glm::mat4 model = GetTransform();
        glm::vec2 half = size * 0.5f;

        switch (type) {
            case ShapeType::Triangle: {
    glm::vec2 half = size * 0.5f;

    // Üçgenin tepe noktalarını merkez etrafında tanımla
    glm::vec2 a = glm::vec2(0.0f,  half.y);     // üst
    glm::vec2 b = glm::vec2(-half.x, -half.y);  // sol alt
    glm::vec2 c = glm::vec2( half.x, -half.y);  // sağ alt

    // Geometrik merkezini bul
    glm::vec2 centroid = (a + b + c) / 3.0f;

    // Vertexleri merkeze göre kaydır
    a -= centroid;
    b -= centroid;
    c -= centroid;

    renderer.DrawTriangle(a, b, c, color, GetTransform());
    break;
}



            case ShapeType::Quad: {
                glm::vec2 p1 = {-half.x, -half.y};
                glm::vec2 p2 = { half.x, -half.y};
                glm::vec2 p3 = { half.x,  half.y};
                glm::vec2 p4 = {-half.x,  half.y};
                renderer.DrawQuad(p1, p2, p3, p4, color, model);
                break;
            }
            default:
                break;
        }

        Node2D::Draw(renderer);
    }
};

class Camera2D : public Node2D {
public:
    float width, height;
    float zoom = 1.0f;

    Camera2D(float w, float h) : width(w), height(h) {}

    glm::mat4 GetViewProj() const {
        float halfW = (width * 0.5f) / zoom;
        float halfH = (height * 0.5f) / zoom;

        glm::mat4 projection = glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-position, 0.0f));
        return projection * view;
    }
};
class Scene {
public:
    std::vector<Node2D*> nodes;
    Camera2D* camera = nullptr;

    void AddNode(Node2D* node) {
        nodes.push_back(node);
    }

    void Update(float dt) {
        for (auto* n : nodes) n->Update(dt);
    }

    void Render(Renderer& renderer) {
        if (!camera) return;
        renderer.Begin(camera->GetViewProj());
        for (auto* n : nodes)
            n->Draw(renderer);
        renderer.End();
    }
};
