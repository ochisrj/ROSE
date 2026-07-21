#pragma once

#include <iostream>
#include <glfw/glfw3.h>

#include "imgui.h"

#include "file.h"
#include "mesh.h"

struct GLFWwindow;


class menubar {
public:
	static void Draw(GLFWwindow* window);
};