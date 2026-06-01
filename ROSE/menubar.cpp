#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "menubar.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "implot.h"
#include "implot_internal.h"

#include "implot3d.h"
#include "implot3d_internal.h"

#include <iostream>

void MenuBar::Draw(GLFWwindow * window)
{
	if (ImGui::BeginMainMenuBar())
	{
		FileMenu::DrawMenu(window);
		EditMenu::DrawMenu();
		ViewMenu::DrawMenu();
		SceneMenu::DrawMenu();
		ToolMenu::DrawMenu();
		WindowMenu::DrawMenu();
		BuildMenu::DrawMenu();
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Text("| %.1f FPS", io.Framerate);

		ImGui::EndMainMenuBar();
	}
	FileMenu::DrawWindow();
	SceneMenu::DrawWindow();
	ToolMenu::DrawWindow();
}