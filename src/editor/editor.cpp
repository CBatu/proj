#include "editor/editor.h"
#include "gfx/renderer.h"
#include "Node/Node2D.h"
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

// === GLOBALLER ===
static Renderer3D renderer;
static CameraNode3D* editorCamera = nullptr;
static Node3D* root = nullptr;
static Node3D* selectedNode = nullptr;

// === Yardımcı fonksiyon: Node listesi ===
static void DrawNodeHierarchy(Node3D* node, int& counter) {
    for (auto* child : node->children) {
        char label[64];
        snprintf(label, sizeof(label), "Node %d", counter++);

        bool isSelected = (child == selectedNode);
        if (ImGui::Selectable(label, isSelected))
            selectedNode = child;

        if (!child->children.empty()) {
            ImGui::Indent();
            DrawNodeHierarchy(child, counter);
            ImGui::Unindent();
        }
    }
}

// ===============================
// === EDITÖR UYGULAMASI ===
// ===============================

void Editor::OnInit() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    renderer.Init();

    // Root node
    root = new Node3D();

    // Kamera
    editorCamera = new CameraNode3D();
    editorCamera->position = {0.0f, 0.0f, 5.0f};
    editorCamera->LookAt({0.0f, 0.0f, 0.0f});

}

void Editor::OnUpdate(float dt) {
    GLFWwindow* window = glfwGetCurrentContext();
    float speed = 5.0f * dt;

    glm::vec3 forward = editorCamera->GetForward();
    glm::vec3 right   = editorCamera->GetRight();

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        editorCamera->position += forward * speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        editorCamera->position -= forward * speed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        editorCamera->position -= right * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        editorCamera->position += right * speed;
}

void Editor::OnRender() {
    // --- ImGui frame başlat ---
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Kullanılacak ölçüler (ImGui ve framebuffer)
    ImVec2 imguiDisplaySize = ImGui::GetIO().DisplaySize; // ImGui koord (genelde pencere boyutu)
    GLFWwindow* window = glfwGetCurrentContext();
    int fb_w = 0, fb_h = 0;
    glfwGetFramebufferSize(window, &fb_w, &fb_h);          // framebuffer boyutu (pixel)
    // scale: ImGui koordinat -> framebuffer pixel
    float scaleX = imguiDisplaySize.x > 0.0f ? (float)fb_w / imguiDisplaySize.x : 1.0f;
    float scaleY = imguiDisplaySize.y > 0.0f ? (float)fb_h / imguiDisplaySize.y : 1.0f;

    ImVec2 displaySize = imguiDisplaySize;

    // === Sol Panel ===
    const float leftW = 220.0f;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(leftW, displaySize.y));
    ImGui::Begin("Scene Hierarchy", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    if (ImGui::Button("Add Cube")) {
        auto cube = new MeshNode3D();
        cube->position = {0, 0, 0};
        root->AddChild(cube);
    }

    if (ImGui::Button("Add Light")) {
        auto light = new LightNode3D();
        light->type = LightNode3D::Type::Point;
        root->AddChild(light);
    }

    ImGui::Separator();
    int counter = 0;
    DrawNodeHierarchy(root, counter);

    if (selectedNode && selectedNode != root) {
        if (ImGui::Button("Delete Selected")) {
            auto& children = root->children;
            children.erase(std::remove(children.begin(), children.end(), selectedNode), children.end());
            delete selectedNode;
            selectedNode = nullptr;
        }
    }
    ImGui::End();

    // === Sağ Panel ===
    const float rightW = 260.0f;
    ImGui::SetNextWindowPos(ImVec2(displaySize.x - rightW, 0));
    ImGui::SetNextWindowSize(ImVec2(rightW, displaySize.y));
    ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    if (selectedNode && selectedNode != root) {
        ImGui::Text("Transform");
        ImGui::Separator();
        ImGui::DragFloat3("Position", &selectedNode->position.x, 0.1f);
        ImGui::DragFloat3("Rotation", &selectedNode->rotation.x, 0.1f);
        ImGui::DragFloat3("Scale", &selectedNode->scale.x, 0.1f, 0.01f, 10.0f);

        if (auto mesh = dynamic_cast<MeshNode3D*>(selectedNode)) {
            ImGui::Separator();
            ImGui::ColorEdit4("Color", &mesh->color.r);
        }
        if (auto light = dynamic_cast<LightNode3D*>(selectedNode)) {
            ImGui::Separator();
            ImGui::ColorEdit4("Light Color", &light->color.r);
            ImGui::DragFloat("Intensity", &light->intensity, 0.1f, 0.0f, 10.0f);
        }
    } else {
        ImGui::Text("No node selected");
    }

    ImGui::Separator();
    ImGui::Text("Camera");
    ImGui::DragFloat3("Camera Pos", &editorCamera->position.x, 0.1f);
    ImGui::DragFloat("FOV", &editorCamera->fov, 1.0f, 10.0f, 120.0f);

    ImGui::End();

    // === Viewport window (sabit layout, padding 0, no title) ===
    float viewportX = leftW;
    float viewportW = displaySize.x - (leftW + rightW);
    float viewportY = 0;
    float viewportH = displaySize.y;

    ImGui::SetNextWindowPos(ImVec2(viewportX, viewportY));
    ImGui::SetNextWindowSize(ImVec2(viewportW, viewportH));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0)); // Şeffaf arkaplan
    ImGui::Begin("Viewport", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    // Önemli: Viewport'un ImGui pencere pozisyonunu ve boyutunu Begin()'den sonra al
    ImVec2 vp_pos = ImGui::GetWindowPos();    // pencere sol-üst köşesi (ImGui koordinat, üstten başlayarak)
    ImVec2 vp_size = ImGui::GetWindowSize();  // pencere boyutu (ImGui koordinat)

    // Convert ImGui coords -> framebuffer pixels
    int gl_x = (int)std::round(vp_pos.x * scaleX);
    int gl_y = (int)std::round((displaySize.y - (vp_pos.y + vp_size.y)) * scaleY); // OpenGL alt-origin
    int gl_w = (int)std::round(vp_size.x * scaleX);
    int gl_h = (int)std::round(vp_size.y * scaleY);

    // Güvenlik: boyutları pozitif yap
    if (gl_w <= 0) gl_w = 1;
    if (gl_h <= 0) gl_h = 1;

    // Sadece viewport alanına çiz (scissor + viewport)
    glEnable(GL_SCISSOR_TEST);
    glScissor(gl_x, gl_y, gl_w, gl_h);
    glViewport(gl_x, gl_y, gl_w, gl_h);

    // Temizle (sadece viewport içinde)
    glClearColor(0.15f, 0.15f, 0.18f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render sahne
    renderer.Begin(editorCamera->GetViewProj(), editorCamera->GetGlobalTransform());
    root->Render(renderer, editorCamera->GetViewProj());
    renderer.UploadLights();
    renderer.End();

    // disable scissor
    glDisable(GL_SCISSOR_TEST);

    ImGui::PopStyleVar(); // WindowPadding
    ImGui::PopStyleColor();

    ImGui::End();         // Viewport

    // --- ImGui draw ---
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void Editor::OnShutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    delete editorCamera;
    delete root;

    renderer.Destroy();
}
