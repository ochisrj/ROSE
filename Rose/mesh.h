#pragma once
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include "shaderclass.h"
#include "sphere.h"

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

    static bool Triangle;
    static bool Rectangle;
    static bool Circle;
    static bool Cube;
    static bool SphereMesh;

    static float color[4];
    static float rotation[3];

    static Sphere sphereInstance;
};