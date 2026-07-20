#pragma once

#include <iostream>
#include "imgui.h"

#include "file.h"

struct GLFWwindow;

class menubar {
public:
	static void Draw(GLFWwindow* window);
};