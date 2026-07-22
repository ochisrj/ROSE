#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "menubar.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void MenuBar::Draw(GLFWwindow * window)
{
    if (ImGui::BeginMainMenuBar())
    {
        FileMenu::DrawMenu();
        MeshMenu::DrawMenu();
        ImGui::EndMainMenuBar();
    }

    MeshMenu::DrawWindow();
}