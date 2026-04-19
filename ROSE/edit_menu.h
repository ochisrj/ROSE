#pragma once

#include <iostream>

#include "imgui.h"

class EditMenu
{
public: 
	static void DrawMenu();
private:
	static bool Undo;
	static bool Redo;
	static bool SelectAll;
};