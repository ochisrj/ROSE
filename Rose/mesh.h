#pragma once
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include "shaderclass.h"

enum class ShapeType {
    NONE,
    TRIANGLE,
    CIRCLE,
    SPHERE,
    CUBE,
    RECTANGLE
};

class MeshMenu
{
public:
    static void Init();
    static void Cleanup();
    static void DrawMenu();
    static void DrawWindow();
    static void DrawGL(Shader& shader);

private:
    static ShapeType currentShape;
    static void SelectShape(ShapeType type);
    static void ClearBuffers();
    static void SetupSphereMesh();

    // Menu Item Flags
    static bool Triangle;
    static bool Rectangle;
    static bool Circle;
    static bool Cube;
    static bool Sphere;

    // Customization Variables
    static float color[4];
    static float rotation[3];

    // OpenGL Buffer Objects
    static unsigned int sphereVAO;
    static unsigned int sphereVBO;
    static unsigned int sphereEBO;
    static int sphereIndexCount;
};