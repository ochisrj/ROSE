#pragma once

#include "imgui.h"

#include <iostream>

class FileMenu {
public:
	static void DrawMenu(GLFWwindow*window);
	static void DrawWindow();
private:
	static bool NewScene;
	static bool OpenScene;
	static bool SaveScene;
	static bool SaveAsScene;
	static bool ImportAsset;
	static bool ExportAsset;
	static bool Preferences;
	static bool VsyncOnOff;
	static bool ProjectSettings;
	static bool Quit;
};