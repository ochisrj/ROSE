#include "mesh.h"

ShapeType MeshMenu::currentShape = ShapeType::NONE;

bool MeshMenu::Triangle = false;
bool MeshMenu::Circle = false;
bool MeshMenu::SphereMesh = false;
bool MeshMenu::Cube = false;
bool MeshMenu::Rectangle = false;

float MeshMenu::color[4] = { 0.2f, 0.7f, 0.9f, 1.0f };
float MeshMenu::rotation[3] = { 0.0f, 0.0f, 0.0f };

Sphere MeshMenu::sphereInstance;

void MeshMenu::Init()
{
}

void MeshMenu::Cleanup()
{
    ClearBuffers();
}

void MeshMenu::ClearBuffers()
{
    sphereInstance.Cleanup();
}

void MeshMenu::SelectShape(ShapeType type)
{
    ClearBuffers();
    currentShape = type;

    Triangle = (type == ShapeType::TRIANGLE);
    Circle = (type == ShapeType::CIRCLE);
    SphereMesh = (type == ShapeType::SPHERE);
    Cube = (type == ShapeType::CUBE);
    Rectangle = (type == ShapeType::RECTANGLE);

    if (type == ShapeType::SPHERE)
    {
        sphereInstance.Init();
    }
}

void MeshMenu::DrawMenu()
{
    if (ImGui::BeginMenu("mesh"))
    {
        if (ImGui::MenuItem("triangle", NULL, &Triangle)) { SelectShape(Triangle ? ShapeType::TRIANGLE : ShapeType::NONE); }
        if (ImGui::MenuItem("Circle", NULL, &Circle)) { SelectShape(Circle ? ShapeType::CIRCLE : ShapeType::NONE); }
        if (ImGui::MenuItem("Sphere", NULL, &SphereMesh)) { SelectShape(SphereMesh ? ShapeType::SPHERE : ShapeType::NONE); }
        if (ImGui::MenuItem("Cube", NULL, &Cube)) { SelectShape(Cube ? ShapeType::CUBE : ShapeType::NONE); }
        if (ImGui::MenuItem("Rectangle", NULL, &Rectangle)) { SelectShape(Rectangle ? ShapeType::RECTANGLE : ShapeType::NONE); }

        ImGui::EndMenu();
    }
}

void MeshMenu::DrawWindow()
{
    if (currentShape == ShapeType::NONE) return;

    ImGui::SeparatorText("Shape Customization");

    if (currentShape == ShapeType::SPHERE) ImGui::Text("Active Shape: Sphere");
    else if (currentShape == ShapeType::TRIANGLE) ImGui::Text("Active Shape: Triangle");
    else if (currentShape == ShapeType::CIRCLE) ImGui::Text("Active Shape: Circle");
    else if (currentShape == ShapeType::CUBE) ImGui::Text("Active Shape: Cube");
    else if (currentShape == ShapeType::RECTANGLE) ImGui::Text("Active Shape: Rectangle");

    ImGui::ColorEdit4("Shape Color", color);
    ImGui::SliderFloat("Shape Rotate X", &rotation[0], 0.0f, 360.0f);
    ImGui::SliderFloat("Shape Rotate Y", &rotation[1], 0.0f, 360.0f);
    ImGui::SliderFloat("Shape Rotate Z", &rotation[2], 0.0f, 360.0f);

    if (ImGui::Button("Reset Shape Transform"))
    {
        rotation[0] = rotation[1] = rotation[2] = 0.0f;
    }
}

void MeshMenu::DrawGL(Shader& shader)
{
    if (currentShape == ShapeType::NONE) return;

    if (currentShape == ShapeType::SPHERE)
    {
        sphereInstance.Draw(shader, color, rotation);
    }
}