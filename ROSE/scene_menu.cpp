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

#include "scene_menu.h"

bool SceneMenu::Camera = false;

void SceneMenu::DrawMenu()
{
	if (ImGui::BeginMenu("Scene"))
	{
		if(ImGui::MenuItem("caamera",NULL,&Camera)){}
		ImGui::EndMenu();
	}
}

void SceneMenu::DrawWindow()
{
	if (Camera) {
		ImGui::Begin("Tasda",&Camera);
		ImGui::Text("afaf");
		ImGui::End();
	}
}