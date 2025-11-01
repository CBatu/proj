#include "core/input.h"
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <glm/glm.hpp>

// === Statik değişkenler ===
GLFWwindow* Input::s_Window = nullptr;
std::unordered_map<int, bool> Input::s_CurrentKeys;
std::unordered_map<int, bool> Input::s_PreviousKeys;
std::unordered_map<int, bool> Input::s_CurrentMouse;
std::unordered_map<int, bool> Input::s_PreviousMouse;

glm::vec2 Input::s_MousePos(0.0f);
glm::vec2 Input::s_LastMousePos(0.0f);
glm::vec2 Input::s_MouseDelta(0.0f);
float Input::s_ScrollDelta = 0.0f;
bool Input::s_FirstMouse = true;

// === Scroll callback ===
static void ScrollCallback(GLFWwindow*, double /*xoffset*/, double yoffset) {
    Input::s_ScrollDelta = static_cast<float>(yoffset);
}

// === Init ===
void Input::Init(GLFWwindow* window) {
    s_Window = window;
    glfwSetScrollCallback(window, ScrollCallback);

    double x, y;
    glfwGetCursorPos(window, &x, &y);
    s_MousePos = glm::vec2((float)x, (float)y);
    s_LastMousePos = s_MousePos;
    s_FirstMouse = true;
}

// === Update ===
void Input::Update() {
    if (!s_Window) return;

    // Önceki durumları sakla
    s_PreviousKeys = s_CurrentKeys;
    s_PreviousMouse = s_CurrentMouse;

    // Scroll bir frame’lik, her frame sıfırlanmalı
    s_ScrollDelta = 0.0f;

    // Tüm tuşları kontrol et (performans için sınırlı aralık yeterli)
    for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key)
        s_CurrentKeys[key] = glfwGetKey(s_Window, key) == GLFW_PRESS;

    // Tüm mouse butonlarını kontrol et
    for (int btn = GLFW_MOUSE_BUTTON_1; btn <= GLFW_MOUSE_BUTTON_LAST; ++btn)
        s_CurrentMouse[btn] = glfwGetMouseButton(s_Window, btn) == GLFW_PRESS;

    // === Mouse hareketi ===
    double mx, my;
    glfwGetCursorPos(s_Window, &mx, &my);

    if (s_FirstMouse) {
        s_MousePos = glm::vec2((float)mx, (float)my);
        s_LastMousePos = s_MousePos;
        s_MouseDelta = {0.0f, 0.0f};
        s_FirstMouse = false;
    } else {
        s_LastMousePos = s_MousePos;
        s_MousePos = glm::vec2((float)mx, (float)my);
        s_MouseDelta = s_MousePos - s_LastMousePos;
    }
}

// === Klavye ===
bool Input::IsKeyDown(int key) { return s_CurrentKeys[key]; }
bool Input::WasKeyPressed(int key) { return s_CurrentKeys[key] && !s_PreviousKeys[key]; }
bool Input::WasKeyReleased(int key) { return !s_CurrentKeys[key] && s_PreviousKeys[key]; }

// === Fare ===
bool Input::IsMouseButtonDown(int button) { return s_CurrentMouse[button]; }
bool Input::WasMouseButtonPressed(int button) { return s_CurrentMouse[button] && !s_PreviousMouse[button]; }
bool Input::WasMouseButtonReleased(int button) { return !s_CurrentMouse[button] && s_PreviousMouse[button]; }

glm::vec2 Input::GetMousePos() { return s_MousePos; }
glm::vec2 Input::GetMouseDelta() { return s_MouseDelta; }
float Input::GetScrollDelta() { return s_ScrollDelta; }

// === Cursor kontrolü ===
void Input::SetCursorMode(CursorMode mode) {
    if (!s_Window) return;

    int glfwMode = GLFW_CURSOR_NORMAL;
    switch (mode) {
        case CursorMode::Normal: glfwMode = GLFW_CURSOR_NORMAL; break;
        case CursorMode::Hidden: glfwMode = GLFW_CURSOR_HIDDEN; break;
        case CursorMode::Locked: glfwMode = GLFW_CURSOR_DISABLED; break;
    }

    glfwSetInputMode(s_Window, GLFW_CURSOR, glfwMode);
}
