#pragma once
#include <iostream>
#include "imgui.h"

class FileMenu
{
public:
	static void DrawMenu();
	static void DrawWindow();
private:
	static bool LoadFile;
	static bool SaveFile;
	static bool SaveAsFile;

	static bool NewFile;
	static bool NewFolderFile;

	static bool ExitFile;
};