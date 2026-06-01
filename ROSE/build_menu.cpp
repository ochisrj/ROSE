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

#include "build_menu.h"

bool BuildMenu::PlayGame = false;
bool BuildMenu::PauseGame = false;
bool BuildMenu::BuildGame = false;
bool BuildMenu::BuildSetting = false;

void BuildMenu::DrawMenu()
{
	if (ImGui::BeginMenu("Build"))
	{
		if (ImGui::MenuItem("Play", "ctrl+F5", &PlayGame)) {}
		if (ImGui::MenuItem("Pause", "F3", &PauseGame)) {}
		if (ImGui::MenuItem("Build", "ctrl+b", &BuildGame)) {}
		if (ImGui::MenuItem("Build Setting", NULL, &BuildSetting)) {}
		ImGui::EndMenu();
	}
}