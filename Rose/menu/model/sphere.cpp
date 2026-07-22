#include "sphere.h"
#include <vector>
#include <cmath>

Sphere::Sphere() : VAO(0), VBO(0), EBO(0), indexCount(0)
{
}

Sphere::~Sphere()
{
    Cleanup();
}

void Sphere::Init()
{
    if (VAO == 0)
    {
        setupMesh();
    }
}

void Sphere::Cleanup()
{
    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        VAO = VBO = EBO = 0;
        indexCount = 0;
    }
}

void Sphere::setupMesh()
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    const unsigned int X_SEGMENTS = 30;
    const unsigned int Y_SEGMENTS = 30;
    const float PI = 3.14159265359f;

    for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
    {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            float xSeg = (float)x / (float)X_SEGMENTS;
            float ySeg = (float)y / (float)Y_SEGMENTS;

            float xPos = std::cos(xSeg * 2.0f * PI) * std::sin(ySeg * PI);
            float yPos = std::cos(ySeg * PI);
            float zPos = std::sin(xSeg * 2.0f * PI) * std::sin(ySeg * PI);

            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);
            vertices.push_back(xSeg);
            vertices.push_back(ySeg);
        }
    }

    for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
    {
        for (unsigned int x = 0; x < X_SEGMENTS; ++x)
        {
            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            indices.push_back(y * (X_SEGMENTS + 1) + x);
            indices.push_back(y * (X_SEGMENTS + 1) + x + 1);

            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            indices.push_back(y * (X_SEGMENTS + 1) + x + 1);
            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x + 1);
        }
    }

    indexCount = static_cast<int>(indices.size());

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Sphere::Draw(Shader& shader, const float color[4], const float rotation[3])
{
    if (VAO == 0) return;

    shader.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));
    shader.setMat4("model", model);

    GLint colorLoc = glGetUniformLocation(shader.ID, "colorOverride");
    if (colorLoc != -1)
    {
        glUniform4f(colorLoc, color[0], color[1], color[2], color[3]);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}