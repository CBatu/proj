
#include "core/engine.h"
#include <iostream>

Engine::Engine(int width, int height, const std::string& title)
    : width(width), height(height), title(title) {
    if (!Init()) {
        std::cerr << "Engine init failed!\n";
        running = false;
    }
}

Engine::~Engine() {
    Shutdown();
}

bool Engine::Init() {
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        return false;
    }

    // ---- GLFW pencere ayarları ----
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // macOS zorunlu
#endif
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // 👈 pencereyi görünmez başlat

    // ---- Pencere oluştur ----
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return false;
    }

    // ---- OpenGL context'i aktif et ----
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync açık

    // ---- GLAD yükle ----
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD\n";
        return false;
    }

    // ---- Framebuffer (retina desteğiyle) ----
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);

    // ---- Viewport değişim callback ----
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int w, int h) {
        glViewport(0, 0, w, h);
    });

    // ---- OpenGL ayarları ----
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ---- Pencereyi göster ----
    glfwShowWindow(window); // 👈 her şey hazır olunca göster

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Framebuffer size: " << fbWidth << "x" << fbHeight << std::endl;

    running = true;
    return true;
}

void Engine::Run() {
    game = new Game();
    game->OnInit();

    auto lastTime = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window) && running) {
        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;

        ProcessInput();
        game->OnUpdate(dt);

        glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        game->OnRender();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    game->OnShutdown();
    delete game;
}

void Engine::ProcessInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        running = false;
}

void Engine::Update() {
    // game logic
}

void Engine::Render() {
    glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Engine::Shutdown() {
    glfwDestroyWindow(window);
    glfwTerminate();
}
