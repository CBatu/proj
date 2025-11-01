#include "core/engine.h"
#include "core/input.h"
#include <iostream>

#include "core/application.h" // IApplication interface tanımı burada

Engine::Engine(int width, int height, const std::string& title, IApplication* app)
    : width(width), height(height), title(title), game(app) {
    if (!Init()) {
        std::cerr << "[Engine] Initialization failed!\n";
        running = false;
    }
}

Engine::~Engine() {
    Shutdown();
}

bool Engine::Init() {
    if (!glfwInit()) {
        std::cerr << "[Engine] Failed to init GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Gizli başlat (UI hazır olunca göster)
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "[Engine] Failed to create window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[Engine] Failed to initialize GLAD\n";
        return false;
    }

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int w, int h) {
        glViewport(0, 0, w, h);
    });

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Input sistemi bağla
    Input::Init(window);

    glfwShowWindow(window);

    std::cout << "✅ Engine initialized (OpenGL " << glGetString(GL_VERSION) << ")\n";
    running = true;
    lastFrameTime = std::chrono::high_resolution_clock::now();
    return true;
}

void Engine::Run() {
    if (!game) {
        std::cerr << "[Engine] No game attached!\n";
        return;
    }

    game->OnInit();

    while (!glfwWindowShouldClose(window) && running) {
        ProcessSystemEvents();

        // Delta time hesapla
        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(now - lastFrameTime).count();
        lastFrameTime = now;

        BeginFrame(dt);
        game->OnUpdate(dt);
        game->OnRender();
        EndFrame();
    }

    game->OnShutdown();
    delete game;
}

void Engine::BeginFrame(float dt) {
    Input::Update();

    glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Engine::EndFrame() {
    glfwSwapBuffers(window);
}

void Engine::ProcessSystemEvents() {
    glfwPollEvents();
    if (Input::IsKeyDown(GLFW_KEY_ESCAPE))
        running = false;
}

void Engine::Shutdown() {
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}
