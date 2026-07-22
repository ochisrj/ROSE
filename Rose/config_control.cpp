#include "config_control.h"
#include "camera.h"
#include "mesh.h" // เพิ่ม include mesh.h
#include "imgui.h"
#include <GLFW/glfw3.h>

// ดึงตัวแปร camera จาก main.cpp มาใช้งานร่วมกัน
extern Camera camera;

// Initialize static members
bool ConfigControl::ConfigMenu = false;
float ConfigControl::clearColor[4] = { 0.10f, 0.10f, 0.12f, 1.0f };
bool ConfigControl::wireframemode = false;
bool ConfigControl::showMyWindow = true;
bool ConfigControl::showMultipleCubes = true;
bool ConfigControl::autoRotate = false;
float ConfigControl::rotateX = 0.0f;
float ConfigControl::rotateY = 0.0f;
float ConfigControl::rotateZ = 0.0f;

float ConfigControl::GetCurrentRotY()
{
    return autoRotate ? static_cast<float>(glfwGetTime()) * 50.0f : rotateY;
}

void ConfigControl::DrawWindow()
{
    if (ImGui::IsKeyPressed(ImGuiKey_5))
    {
        showMyWindow = !showMyWindow;
    }

    if (!showMyWindow) return;

    ImGuiWindowFlags ImGuiFlag =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoScrollbar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_SeparatorTextBorderSize, 1.0f);

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 workPos = viewport->WorkPos;
    ImVec2 workSize = viewport->WorkSize;
    ImGui::SetNextWindowPos(ImVec2(workPos.x + workSize.x, workPos.y), ImGuiCond_Always, ImVec2(1.01f, -0.02f));
    ImGui::SetNextWindowSize(ImVec2(500.0f, 800.0f), ImGuiCond_FirstUseEver);

    ImGui::Begin("My Window", &showMyWindow, ImGuiFlag);
    ImGui::SeparatorText("Control Tips (Press 5 to hide)");
    ImGui::Text("W = Move Forward");
    ImGui::Text("S = Move Backward");
    ImGui::Text("A = Move Left");
    ImGui::Text("D = Move Right");
    ImGui::Text("SpaceBar = Move Up");
    ImGui::Text("Left Ctrl = Move Down");
    ImGui::Text("Middle Mouse = Zoom");
    ImGui::Text("Right Click = Drag Camera");

    ImGui::SeparatorText("Camera & Scene Control");
    ImGui::ColorEdit4("Background", clearColor);
    ImGui::Checkbox("Wireframe", &wireframemode);
    ImGui::Checkbox("Show 10 Cubes", &showMultipleCubes);

    static bool vsync = true;
    if (ImGui::Checkbox("Enable VSync", &vsync)) {
        glfwSwapInterval(vsync ? 1 : 0);
    }

    ImGui::Text("Camera Status & Settings");
    ImGui::Text("Pos: (%.2f, %.2f, %.2f)", camera.Position.x, camera.Position.y, camera.Position.z);
    ImGui::Text("Yaw: %.1f | Pitch: %.1f", camera.Yaw, camera.Pitch);
    ImGui::Text("Framerate : %.1f fps", ImGui::GetIO().Framerate);
    ImGui::SliderFloat("Camera Speed", &camera.MovementSpeed, 0.5f, 15.0f);
    ImGui::SliderFloat("Mouse Sensitivity", &camera.MouseSensitivity, 0.01f, 0.5f);
    ImGui::SliderFloat("FOV (Zoom)", &camera.Zoom, 1.0f, 90.0f);

    ImGui::SeparatorText("Rotation Control");
    ImGui::Checkbox("Auto Rotate (Y-Axis)", &autoRotate);
    if (!autoRotate)
    {
        ImGui::SliderFloat("Rotate X", &rotateX, 0.0f, 360.0f);
        ImGui::SliderFloat("Rotate Y", &rotateY, 0.0f, 360.0f);
        ImGui::SliderFloat("Rotate Z", &rotateZ, 0.0f, 360.0f);

        if (ImGui::Button("Reset Rotation"))
        {
            rotateX = 0.0f;
            rotateY = 0.0f;
            rotateZ = 0.0f;
        }
    }

    if (ImGui::Button("Reset Camera Position"))
    {
        camera.Position = glm::vec3(0.0f, 0.0f, 3.0f);
        camera.Yaw = -90.0f;
        camera.Pitch = 0.0f;
    }

    // วาด Shape Customization รวมในหน้าต่างนี้
    MeshMenu::DrawWindow();

    ImGui::End();
    ImGui::PopStyleVar(2);
}