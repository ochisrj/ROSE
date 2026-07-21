#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "mesh.h"

bool MeshMenu::Triangle = false;
bool MeshMenu::Circle = false;
bool MeshMenu::Sphere = false;
bool MeshMenu::Cube = false;
bool MeshMenu::Rectangle = false;

void MeshMenu::DrawMenu()
{
	if (ImGui::BeginMenu("mesh"))
	{
		if(ImGui::MenuItem("triangle",NULL,&Triangle)){}
		if(ImGui::MenuItem("Circle",NULL,&Circle)){}
		if(ImGui::MenuItem("Sphere",NULL,&Sphere)){}
		if(ImGui::MenuItem("Cube",NULL,&Cube)){}
		if(ImGui::MenuItem("Rectangle",NULL,&Rectangle)){}
		ImGui::EndMenu();
	}
}

void MeshMenu::DrawWindow()
{

}

void MeshMenu::DrawGL()
{

}