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

class filemenu
{
public:
	filemenu();
private:
	bool newscene_item = false;
	bool openscene_item = false;
	bool save_item = false;
	bool saveas_item = false;
	bool importasset_item = false;
	bool exportasset_item = false;
	bool preferences_item = false;
	bool projectsetting_item = false;
	bool quit_item = false;

	void DrawNewScene();
	void DrawOpenScene();
	void DrawSave();
	void DrawSaveAs();
	void DrawImportAsset();
	void DrawExportAsset();
	void DrawPreferences();
	void DrawProjectSetting();
	void DrawQuit();
};