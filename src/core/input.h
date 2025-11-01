#pragma once
#include <glm/glm.hpp>
#include <unordered_map>
struct GLFWwindow;

enum class CursorMode {
    Normal,
    Hidden,
    Locked
};

class Input {
public:
    static void Init(GLFWwindow* window);
    static void Update();

    // Klavye
    static bool IsKeyDown(int key);
    static bool WasKeyPressed(int key);
    static bool WasKeyReleased(int key);

    // Fare
    static bool IsMouseButtonDown(int button);
    static bool WasMouseButtonPressed(int button);
    static bool WasMouseButtonReleased(int button);

    static glm::vec2 GetMousePos();
    static glm::vec2 GetMouseDelta();
    static float GetScrollDelta();

    static void SetCursorMode(CursorMode mode);
    static float s_ScrollDelta;

private:
    static GLFWwindow* s_Window;

    static std::unordered_map<int, bool> s_CurrentKeys;
    static std::unordered_map<int, bool> s_PreviousKeys;

    static std::unordered_map<int, bool> s_CurrentMouse;
    static std::unordered_map<int, bool> s_PreviousMouse;

    static glm::vec2 s_MousePos;
    static glm::vec2 s_LastMousePos;
    static glm::vec2 s_MouseDelta;
    
    static bool s_FirstMouse;
};
