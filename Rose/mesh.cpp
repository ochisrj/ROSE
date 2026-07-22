#include "mesh.h"
#include <vector>
#include <cmath>

// Static Variable Initializations
ShapeType MeshMenu::currentShape = ShapeType::NONE;

bool MeshMenu::Triangle = false;
bool MeshMenu::Circle = false;
bool MeshMenu::Sphere = false;
bool MeshMenu::Cube = false;
bool MeshMenu::Rectangle = false;

float MeshMenu::color[4] = { 0.2f, 0.7f, 0.9f, 1.0f };
float MeshMenu::rotation[3] = { 0.0f, 0.0f, 0.0f };

unsigned int MeshMenu::sphereVAO = 0;
unsigned int MeshMenu::sphereVBO = 0;
unsigned int MeshMenu::sphereEBO = 0;
int MeshMenu::sphereIndexCount = 0;

void MeshMenu::Init()
{
}

void MeshMenu::Cleanup()
{
    ClearBuffers();
}

void MeshMenu::ClearBuffers()
{
    if (sphereVAO != 0) {
        glDeleteVertexArrays(1, &sphereVAO);
        glDeleteBuffers(1, &sphereVBO);
        glDeleteBuffers(1, &sphereEBO);
        sphereVAO = sphereVBO = sphereEBO = 0;
        sphereIndexCount = 0;
    }
}

void MeshMenu::SelectShape(ShapeType type)
{
    // 🌟 1. ล้าง Shape และ Buffer เก่าทันทีเมื่อมีการสลับ
    ClearBuffers();
    currentShape = type;

    // 🌟 2. Sync สถานะ Checkbox ของ ImGui
    Triangle = (type == ShapeType::TRIANGLE);
    Circle = (type == ShapeType::CIRCLE);
    Sphere = (type == ShapeType::SPHERE);
    Cube = (type == ShapeType::CUBE);
    Rectangle = (type == ShapeType::RECTANGLE);

    // 🌟 3. สปอว์น Mesh ของรูปทรงใหม่
    if (type == ShapeType::SPHERE)
    {
        SetupSphereMesh();
    }
}

void MeshMenu::DrawMenu()
{
    if (ImGui::BeginMenu("mesh"))
    {
        if (ImGui::MenuItem("triangle", NULL, &Triangle)) { SelectShape(Triangle ? ShapeType::TRIANGLE : ShapeType::NONE); }
        if (ImGui::MenuItem("Circle", NULL, &Circle)) { SelectShape(Circle ? ShapeType::CIRCLE : ShapeType::NONE); }
        if (ImGui::MenuItem("Sphere", NULL, &Sphere)) { SelectShape(Sphere ? ShapeType::SPHERE : ShapeType::NONE); }
        if (ImGui::MenuItem("Cube", NULL, &Cube)) { SelectShape(Cube ? ShapeType::CUBE : ShapeType::NONE); }
        if (ImGui::MenuItem("Rectangle", NULL, &Rectangle)) { SelectShape(Rectangle ? ShapeType::RECTANGLE : ShapeType::NONE); }

        ImGui::EndMenu();
    }
}

void MeshMenu::DrawWindow()
{
    if (currentShape == ShapeType::NONE) return;

    ImGui::Begin("Shape Customization");

    if (currentShape == ShapeType::SPHERE) ImGui::Text("Active Shape: Sphere");
    else if (currentShape == ShapeType::TRIANGLE) ImGui::Text("Active Shape: Triangle");
    else if (currentShape == ShapeType::CIRCLE) ImGui::Text("Active Shape: Circle");
    else if (currentShape == ShapeType::CUBE) ImGui::Text("Active Shape: Cube");
    else if (currentShape == ShapeType::RECTANGLE) ImGui::Text("Active Shape: Rectangle");

    ImGui::Separator();

    // 🌟 ImGui Control: สี และ หมุน X, Y, Z
    ImGui::ColorEdit4("Color", color);
    ImGui::SliderFloat("Rotate X", &rotation[0], 0.0f, 360.0f);
    ImGui::SliderFloat("Rotate Y", &rotation[1], 0.0f, 360.0f);
    ImGui::SliderFloat("Rotate Z", &rotation[2], 0.0f, 360.0f);

    if (ImGui::Button("Reset Transform"))
    {
        rotation[0] = rotation[1] = rotation[2] = 0.0f;
    }

    ImGui::End();
}

void MeshMenu::DrawGL(Shader& shader)
{
    if (currentShape == ShapeType::NONE) return;

    shader.use();

    // Transform Matrix
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

    if (currentShape == ShapeType::SPHERE && sphereVAO != 0)
    {
        glBindVertexArray(sphereVAO);
        glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

// 🌟 UV Sphere Mesh Generator
void MeshMenu::SetupSphereMesh()
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

            // Pos (3) + UV (2)
            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);
            vertices.push_back(xSeg);
            vertices.push_back(ySeg);
        }
    }

    bool oddRow = false;
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

    sphereIndexCount = static_cast<int>(indices.size());

    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);

    glBindVertexArray(sphereVAO);

    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // UV
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}