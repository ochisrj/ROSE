#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include "implot3d.h"
#include "implot_internal.h"
#include "implot3d_internal.h"

#include "file_menu.h"

bool FileMenu::NewScene = false;
bool FileMenu::OpenScene = false;
bool FileMenu::SaveScene = false;
bool FileMenu::SaveAsScene = false;
bool FileMenu::ImportAsset = false;
bool FileMenu::ExportAsset = false;
bool FileMenu::Preferences = false;
bool FileMenu::VsyncOnOff = false;
bool FileMenu::ProjectSettings = false;
bool FileMenu::Quit = false;
    
void FileMenu::DrawMenu(GLFWwindow * window)
{
    if (ImGui::BeginMenu("File")) {
        if(ImGui::MenuItem("New Scene",NULL,&NewScene)) {}
        if(ImGui::MenuItem("Open Scene",NULL,&OpenScene)) {}
        if(ImGui::MenuItem("Save",NULL,&SaveScene)) {}
        if(ImGui::MenuItem("Save As",NULL,&SaveAsScene)){}
        ImGui::Separator();
        if(ImGui::MenuItem("Import Asset",NULL,&ImportAsset)) {}
        if(ImGui::MenuItem("Export Asset",NULL,&ExportAsset)) {}
        ImGui::Separator();
        if(ImGui::MenuItem("Preference",NULL,&Preferences)){}
        if(ImGui::MenuItem("Project Setting",NULL,&ProjectSettings)){}
        ImGui::Separator();
        if(ImGui::MenuItem("Quit",NULL , &Quit)) 
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }


        ImGui::EndMenu();
    }
}

void FileMenu::DrawWindow()
{
   
}