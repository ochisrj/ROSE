#pragma once

#include <iostream>

#include "imgui.h"

class BuildMenu
{
public:
	static void DrawMenu();
private:
	static bool PlayGame;
	static bool PauseGame;
	static bool BuildGame;
	static bool BuildSetting;
};