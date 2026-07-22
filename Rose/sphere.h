#pragma once
#include "Shape.h"
#include <vector>

class SphereShape : public Shape {
private:
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    int indexCount = 0;

public:
    SphereShape();
    ~SphereShape();

    void Init() override;
    void Draw(Shader& shader) override;
    void Cleanup() override;
};