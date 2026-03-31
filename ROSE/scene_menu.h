#pragma once

#include <iostream>

#include "imgui.h"

class SceneMenu
{
public:
	static void Draw();
	static void DrawWindow();
private:
	static bool Camera;
};