#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"

#include "file.h"
#include "mesh.h"

struct GLFWwindow;


class MenuBar {
public:
	static void Draw(GLFWwindow* window);
};