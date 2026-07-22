#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shaderclass.h"

class Sphere
{
public:
    Sphere();
    ~Sphere();

    void Init();
    void Cleanup();
    void Draw(Shader& shader, const float color[4], const float rotation[3]);

private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    int indexCount;

    void setupMesh();
};