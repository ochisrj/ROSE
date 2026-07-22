#pragma once

#include <iostream>
#include <glfw/glfw3.h>

#include "imgui.h"

#include "config_control.h"

struct GLFWwindow;


class viewport 
{
public:
	static void DrawWindow(GLFWwindow* window);
};