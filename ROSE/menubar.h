#pragma once

#include <iostream>
#include "imgui.h"

#include "file_menu.h"
#include "build_menu.h"
#include "edit_menu.h"
#include "tools_menu.h"
#include "scene_menu.h"
#include "view_menu.h"
#include "window_menu.h"
#include "help_menu.h"

class MenuBar
{
public:
	static void Draw();
};