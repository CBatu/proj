#include "editor/editor.h"
#include "gfx/renderer.h"
#include "Node/Node2D.h"
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// === GLOBALLER ===
static Renderer3D renderer;
static CameraNode3D* editorCamera = nullptr;
static Node3D* root = nullptr;
static Node3D* selectedNode = nullptr;

// Framebuffer nesnesi
static GLuint fbo = 0, colorTex = 0, depthRbo = 0;

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

// === Framebuffer oluştur ===
static void CreateFramebuffer(int width, int height) {
    if (fbo) {
        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &colorTex);
        glDeleteRenderbuffers(1, &depthRbo);
    }

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &colorTex);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);

    glGenRenderbuffers(1, &depthRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthRbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer incomplete!\n";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
    //editorCamera->LookAt({0.0f, 0.0f, 0.0f});

    // Başlangıç framebuffer
    CreateFramebuffer(1280, 720);
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

    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

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
            ImGui::ColorEdit4("Color", &mesh->material->color.r);
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

    // === Viewport ===
    ImGui::SetNextWindowPos(ImVec2(leftW, 0));
    ImGui::SetNextWindowSize(ImVec2(displaySize.x - (leftW + rightW), displaySize.y));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    ImVec2 viewportSize = ImGui::GetContentRegionAvail();

    // Framebuffer boyutu değiştiyse yeniden oluştur
    static int fbw = 0, fbh = 0;
    if ((int)viewportSize.x != fbw || (int)viewportSize.y != fbh) {
        fbw = std::max(1, (int)viewportSize.x);
        fbh = std::max(1, (int)viewportSize.y);
        CreateFramebuffer(fbw, fbh);
    }

    // Framebuffer'a render et
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fbw, fbh);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.15f, 0.15f, 0.18f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderer.Begin(editorCamera->GetViewProj(), editorCamera->GetGlobalTransform());
    root->Render(renderer, editorCamera->GetViewProj());
    renderer.UploadLights();
    renderer.End();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Framebuffer içeriğini göster
    ImGui::Image((ImTextureID)(intptr_t)colorTex, viewportSize, ImVec2(0,1), ImVec2(1,0));

    // === Gizmo ===
    if (selectedNode && selectedNode != root) {
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y,
                          ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);

        glm::mat4 view = editorCamera->GetView();
        glm::mat4 proj = editorCamera->GetProjection();
        glm::mat4 model = selectedNode->GetGlobalTransform();

        static ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(ImGuiKey_W)) operation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(ImGuiKey_E)) operation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(ImGuiKey_R)) operation = ImGuizmo::SCALE;

        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(proj),
                             operation, ImGuizmo::WORLD, glm::value_ptr(model));

        if (ImGuizmo::IsUsing()) {
            glm::vec3 pos, rot, scl;
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(model),
                                                  glm::value_ptr(pos),
                                                  glm::value_ptr(rot),
                                                  glm::value_ptr(scl));
            selectedNode->position = pos;
            selectedNode->rotation = glm::radians(rot);
            selectedNode->scale = scl;
        }
    }

    ImGui::End();
    ImGui::PopStyleVar();

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

    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &colorTex);
    glDeleteRenderbuffers(1, &depthRbo);
}
