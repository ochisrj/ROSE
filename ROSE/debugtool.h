#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include "implot.h"
#include "implot_internal.h"

#include "implot3d.h"
#include "implot3d_internal.h"

class debugtool
{
public:
	debugtool();
	void DrawUI(ImGuiIO& io);

private:
	bool text_formatting = false;
	bool demo_window = false;
	bool demo_plot = false;
	bool demo_plot3D = false;
	bool fps_window = false;
	bool menubar = true;
	bool image_viewer = false;
	bool calculator = false;

	void DrawTextFormat();
	void DrawDemo();
	void DrawDemoPlot();
	void DrawDemoPlot3D();
	void DrawFPS();
	void DrawMenuBar();
	void DrawImageViewer();
	void DrawCalculator();
};