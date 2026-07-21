#pragma once
#include <iostream>
#include "imgui.h"

class MeshMenu
{
public:
	static void DrawMenu();
	static void DrawWindow();
	static void DrawGL();

private:
	static bool Triangle;
	static bool Rectangle;
	static bool Circle;
	static bool Cube;
	static bool Sphere;
};