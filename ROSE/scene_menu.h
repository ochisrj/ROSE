#pragma once

#include <iostream>

#include "imgui.h"

class SceneMenu
{
public:
	static void DrawMenu();
	static void DrawWindow();
private:
	static bool Camera;
};