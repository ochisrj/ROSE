#pragma once

#include <iostream>

#include "imgui.h"

class ViewMenu
{
public:
	static void DrawMenu();
	static void DrawWindow();
private:
	static bool AssetBrowser;
	static bool ConsoleLog;
	static bool AnimationEditor;
	static bool FullScreen;
	static bool Windowed;
};