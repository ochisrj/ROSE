#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include "implot3d.h"
#include "implot_internal.h"
#include "implot3d_internal.h"

#include "edit_menu.h"

bool EditMenu::Redo = false;
bool EditMenu::Undo = false;
bool EditMenu::SelectAll = false;

void EditMenu::Draw()
{
	if (ImGui::BeginMenu("Edit"))
	{
		if(ImGui::MenuItem("Undo","ctrl+z",&Undo)) {}
		if(ImGui::MenuItem("Redo","ctrl+x",&Redo)) {}
		if(ImGui::MenuItem("Select All","ctrl+a",&SelectAll)){}
		ImGui::EndMenu();
	}
}