#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


int main() {
    if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW\n";
    return -1;
    }


    // Request OpenGL 3.3 core profile (works on Windows & Linux)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // required on macOS
    #endif


    GLFWwindow* window = glfwCreateWindow(800, 600, "glfw+glad example", NULL, NULL);
    if (!window) {
    std::cerr << "Failed to create GLFW window\n";
    glfwTerminate();
    return -1;
    }


    glfwMakeContextCurrent(window);


    // Load OpenGL function pointers with glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD\n";
    return -1;
    }


    std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << "\n";
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n";


    while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);


    // simple clear color
    glClearColor(0.15f, 0.2f, 0.25f, 1.0f);


    glfwSwapBuffers(window);
    glfwPollEvents();
    }


    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
