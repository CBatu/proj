#pragma once
#include <string>
#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "core/input.h"

class IApplication;

class Engine {
public:
    Engine(int width, int height, const std::string& title, IApplication* app);
    ~Engine();

    void Run();

private:
    bool Init();
    void Shutdown();

    void BeginFrame(float dt);
    void EndFrame();

    void ProcessSystemEvents();

private:
    GLFWwindow* window = nullptr;
    IApplication* game = nullptr;
    bool running = false;

    int width, height;
    std::string title;

    std::chrono::high_resolution_clock::time_point lastFrameTime;
};
