#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include "game/game.h"

class Engine {
public:
    Engine(int width, int height, const std::string& title);
    ~Engine();

    void Run();

private:
    GLFWwindow* window;
    int width, height;
    std::string title;
    bool running = true;
    Game* game = nullptr;

    bool Init();
    void ProcessInput();
    void Update();
    void Render();
    void Shutdown();
};
