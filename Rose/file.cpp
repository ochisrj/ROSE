#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "file.h"

bool FileMenu::LoadFile = false;
bool FileMenu::SaveFile = false;
bool FileMenu::SaveAsFile = false;
bool FileMenu::NewFile = false;
bool FileMenu::NewFolderFile = false;
bool FileMenu::ExitFile = false;

void FileMenu::DrawMenu()
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Load",NULL,&LoadFile)){}
		if (ImGui::MenuItem("Save",NULL,&SaveFile)){}
		if (ImGui::MenuItem("Save as ...", NULL, &SaveAsFile)) {}
		ImGui::Separator();
		if (ImGui::MenuItem("New File", NULL, &NewFile)) {}
		if (ImGui::MenuItem("New Folder", NULL, &NewFolderFile)) {}
		ImGui::Separator();
		if (ImGui::MenuItem("Exit", NULL, &ExitFile)) {}

		ImGui::EndMenu();

	}
}

void FileMenu::DrawWindow()
{

}