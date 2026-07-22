#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shaderclass.h"

class Shape {
public:
    virtual ~Shape() {}
    virtual void Init() = 0;
    virtual void Draw(Shader& shader) = 0;
    virtual void Cleanup() = 0;

    // ตัวแปรการจัดการที่ทุก Shape ต้องมีร่วมกัน
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec4 color = glm::vec4(0.2f, 0.7f, 0.9f, 1.0f);
};