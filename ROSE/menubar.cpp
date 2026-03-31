#include "menubar.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "implot.h"
#include "implot_internal.h"

#include "implot3d.h"
#include "implot3d_internal.h"

#include "menubar.h"

#include <iostream>

void MenuBar::Draw()
{
	if (ImGui::BeginMainMenuBar())
	{
		FileMenu::Draw();
		EditMenu::Draw();
		ViewMenu::Draw();
		SceneMenu::Draw();

		ImGui::EndMainMenuBar();
	}
	SceneMenu::DrawWindow();
}