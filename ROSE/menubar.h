#pragma once

#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include "implot.h"
#include "implot_internal.h"

#include "implot3d.h"
#include "implot3d_internal.h"

#include "file_menu.h"
#include "build_menu.h"
#include "edit_menu.h"
#include "help_menu.h"
#include "scene_menu.h"
#include "tools_menu.h"
#include "view_menu.h"
#include "window_menu.h"


class menubar
{
public:
	menubar();


	void DrawUI(ImGuiIO& io);
private:
	bool menu_bar = true;
	bool buid_menu = true;
	bool edit_menu = true;
	bool file_menu = true;
	bool help_menu = true;
	bool scene_menu = true;
	bool tools_menu = true;
	bool view_menu = true;
	bool window_menu = true;


	void DrawMenuBar();
	void DrawBuildMenu();
	void DrawEditMenu();
	void DrawFileMenu();
	void DrawHelpMenu();
	void DrawSceneMenu();
	void DrawToolsMenu();
	void DrawViewMenu();
	void DrawWindowMenu();

};