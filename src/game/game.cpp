#include "game/game.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "Node/Node2D.h"

Renderer3D renderer;
Node3D* root = nullptr;
glm::mat4 viewProj;
CameraNode3D* camera = nullptr;

float cameraSpeed = 5.0f;      // Birim/saniye
float mouseSensitivity = 0.002f;
double lastMouseX = 640.0, lastMouseY = 360.0;
bool firstMouse = true;

void Game::OnInit() {
    renderer.Init();
    glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Root node
    root = new Node3D();

    auto light = new LightNode3D();
    light->position = {2.0f, 4.0f, 2.0f};
    light->color = {1.0f, 1.0f, 1.0f};
    root->AddChild(light);


    // === Kamera Node ===
    camera = new CameraNode3D();
    camera->position = { 0.0f, 1.0f, 4.0f };
    camera->SetPerspective(60.0f, 1280.0f/720.0f, 0.1f, 100.0f);
    root->AddChild(camera);

    // === Sahne ===
    auto cube1 = new MeshNode3D();
    cube1->position = { -1.0f, 0.0f, 0.0f };
    cube1->color = { 1, 0, 0, 1 };
    root->AddChild(cube1);

    auto cube2 = new MeshNode3D();
    cube2->position = { 1.0f, 0.0f, 0.0f };
    cube2->color = { 0, 0, 1, 1 };
    root->AddChild(cube2);
}

void Game::OnRender() {

    glm::mat4 viewProj = camera->GetViewProj();

    renderer.Begin(viewProj);
    root->Render(renderer, viewProj);
    renderer.End();
}
void Game::OnUpdate(float dt) {
    // Fare hareketi
    double mouseX, mouseY;
    glfwGetCursorPos(glfwGetCurrentContext(), &mouseX, &mouseY);

    if (firstMouse) {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        firstMouse = false;
    }

    float offsetX = float(lastMouseX-mouseX) * mouseSensitivity;
    float offsetY = float(lastMouseY - mouseY) * mouseSensitivity;
    lastMouseX = mouseX;
    lastMouseY = mouseY;

    // Euler açılarını güncelle
    camera->rotation.y += offsetX; // Yaw
    camera->rotation.x += offsetY; // Pitch

    camera->rotation.x = glm::clamp(camera->rotation.x, glm::radians(-89.0f), glm::radians(89.0f));

    // Kamera yön vektörleri: kamera node'unun dönüşünden türet (tüm dönüş mantığıyla eşleşir)
    glm::vec3 front = camera->GetForward();
    glm::vec3 right = camera->GetRight();
    glm::vec3 up = camera->GetUp();

    // Hareket
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS) camera->position += front * cameraSpeed * dt;
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS) camera->position -= front * cameraSpeed * dt;
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A) == GLFW_PRESS) camera->position -= right * cameraSpeed * dt;
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D) == GLFW_PRESS) camera->position += right * cameraSpeed * dt;
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_SPACE) == GLFW_PRESS) camera->position += up * cameraSpeed * dt;
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) camera->position -= up * cameraSpeed * dt;

    root->Update(dt);
}



void Game::OnShutdown() {
    renderer.Destroy();
    delete root;
}
