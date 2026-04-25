#pragma once

#include <iostream>

#include "imgui.h"

class HelpMenu
{
public:
	static void DrawMenu();
	static void DrawWindow();
private:
	static bool Show_About_Window;
	static bool Show_Documentation_Window;
	static bool Show_Report_Window;
	static bool Show_UpdateLog_Window;
};