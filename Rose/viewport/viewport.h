#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"

#include "config_control.h"

struct GLFWwindow;


class viewport
{
public:
	static void DrawWindow(GLFWwindow* window);
};