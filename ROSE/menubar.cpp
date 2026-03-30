#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include "implot.h"
#include "implot_internal.h"

#include "implot3d.h"
#include "implot3d_internal.h"

#include "menubar.h"

#include <iostream>
#include <cmath>
#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <vector>
#include <deque>
#include <ctime>
#include <chrono>
#include <algorithm>

menubar::menubar()
{
	
}

void menubar::DrawUI(ImGuiIO& io)
{
	DrawMenuBar();
	DrawBuildMenu();
	DrawEditMenu();
	DrawFileMenu();
	DrawHelpMenu();
	DrawSceneMenu();
	DrawToolsMenu();
	DrawViewMenu();
	DrawWindowMenu();
}

void menubar::DrawMenuBar()
{
	if(ImGui::BeginMainMenuBar())
	{

	}
}