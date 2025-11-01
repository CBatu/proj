#include "game/game.h"
#include "Node/Node2D.h"
#include "core/input.h"
#include "gfx/renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <iostream>

// === Global ===
static Renderer3D renderer;
static Node3D* root = nullptr;
static CameraNode3D* camera = nullptr;

float cameraSpeed = 5.0f;
float mouseSensitivity = 0.1f;

void Game::OnInit() {
    renderer.Init();

    // === Root node ===
    root = new Node3D();

    // === Işık ===
    auto light = new LightNode3D();
    light->position = { 2.0f, 4.0f, 2.0f };
    light->type = LightNode3D::Type::Point;
    light->color = { 1.0f, 1.0f, 1.0f };
    root->AddChild(light);

    // === Kamera ===
    camera = new CameraNode3D();
    camera->position = { 0.0f, 1.0f, 4.0f };
    camera->SetPerspective(60.0f, 1280.0f / 720.0f, 0.1f, 100.0f);
    root->AddChild(camera);

    Input::SetCursorMode(CursorMode::Locked); // ESC ile çıkabilirsin

    // === Küpler ===
    auto cube1 = new MeshNode3D();
    cube1->position = { -1.0f, 0.0f, 0.0f };
    cube1->color = { 1, 0, 0, 1 };
    root->AddChild(cube1);

    auto cube2 = new MeshNode3D();
    cube2->position = { 1.0f, 0.0f, 0.0f };
    cube2->color = { 0, 0, 1, 1 };
    root->AddChild(cube2);
}

void Game::OnUpdate(float dt) {
    // === Fare hareketi ===
    glm::vec2 mouseDelta = Input::GetMouseDelta();
    camera->yaw   += mouseDelta.x * mouseSensitivity;
    camera->pitch -= mouseDelta.y * mouseSensitivity;

    // Açı sınırları
    camera->pitch = glm::clamp(camera->pitch, -89.0f, 89.0f);

    camera->UpdateDirection();

    // === Hareket ===
    if (Input::IsKeyDown(GLFW_KEY_W)) camera->position += camera->GetForward() * cameraSpeed * dt;
    if (Input::IsKeyDown(GLFW_KEY_S)) camera->position -= camera->GetForward() * cameraSpeed * dt;
    if (Input::IsKeyDown(GLFW_KEY_A)) camera->position -= camera->GetRight()   * cameraSpeed * dt;
    if (Input::IsKeyDown(GLFW_KEY_D)) camera->position += camera->GetRight()   * cameraSpeed * dt;
    if (Input::IsKeyDown(GLFW_KEY_SPACE)) camera->position += glm::vec3(0,1,0) * cameraSpeed * dt;
    if (Input::IsKeyDown(GLFW_KEY_LEFT_CONTROL)) camera->position -= glm::vec3(0,1,0) * cameraSpeed * dt;

    root->Update(dt);
}


void Game::OnRender() {
    glm::mat4 viewProj = camera->GetViewProj();

    renderer.Begin(viewProj, camera->GetGlobalTransform());
    root->Render(renderer, viewProj);
    renderer.UploadLights();
    renderer.End();
}

void Game::OnShutdown() {
    renderer.Destroy();
    delete root;
}
