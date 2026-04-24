#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include "implot3d.h"
#include "implot_internal.h"
#include "implot3d_internal.h"


#include "view_menu.h"

bool ViewMenu::AssetBrowser = false;
bool ViewMenu::ConsoleLog = false;
bool ViewMenu::AnimationEditor = false;
bool ViewMenu::FullScreen = false;
bool ViewMenu::Windowed = false;

void ViewMenu::DrawMenu()
{
	if (ImGui::BeginMenu("View"))
	{
		if (ImGui::MenuItem("Asset Browser",NULL,&AssetBrowser)){}
		if (ImGui::MenuItem("Console Log", NULL, &ConsoleLog)){}
		if (ImGui::MenuItem("Animation Editor",NULL,&AnimationEditor)){}
		ImGui::Separator();
		if (ImGui::MenuItem("Fullscreen", NULL, &FullScreen)) {}
		if(ImGui::MenuItem("Windowled",NULL,&Windowed)){}
		ImGui::EndMenu();
	}
}

void ViewMenu::DrawWindow()
{
	
}