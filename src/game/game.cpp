#include "game/game.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "Node/Node2D.h"

Scene scene;
Renderer renderer;
Camera2D camera(1280, 720);

Shape2D tri, rect;

void Game::OnInit() {
    renderer.Init();

    tri.type = ShapeType::Triangle;
    tri.position = {100, 100};
    tri.size = {150, 150};
    tri.color = {1, 0, 0, 1};
    tri.rotation = glm::radians(30.0f);
    tri.scale = {1.5f, 1.0f};

    rect.type = ShapeType::Quad;
    rect.position = {400, 300};
    rect.size = {200, 150};
    rect.color = {0, 1, 0, 1};
    rect.rotation = glm::radians(90.0f);

    scene.camera = &camera;

    scene.AddNode(&tri);
    scene.AddNode(&rect);
}

void Game::OnRender() {
    scene.Render(renderer);
}

void Game::OnShutdown() {
    renderer.Destroy();
}

void Game::OnUpdate(float dt) {
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.position.y += 500.0f * dt;
    }else if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.position.y -= 500.0f * dt;
    }

    else if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.position.x -= 500.0f * dt;
    }else if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.position.x += 500.0f * dt;
    }if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_Q) == GLFW_PRESS)
    {
        camera.zoom += 5.0f * dt;
    }else if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_E) == GLFW_PRESS)
    {
        camera.zoom -= 5.0f * dt;
    }
}
