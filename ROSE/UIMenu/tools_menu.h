#pragma once

#include <iostream>

#include "imgui.h"

class ToolMenu
{
public:
	static void DrawMenu();
	static void DrawWindow();
private:
	static bool PerformaceHUD;
	static bool ScriptEditor;
};