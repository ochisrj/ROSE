#pragma once
#include <glm/glm.hpp>

class ConfigControl
{
public:
    static bool ConfigMenu;

    static float clearColor[4];
    static bool wireframemode;
    static bool showMyWindow;
    static bool showMultipleCubes;
    static bool autoRotate;
    static float rotateX;
    static float rotateY;
    static float rotateZ;

    // ฟังก์ชันวาด UI
    static void DrawWindow();
    static float GetCurrentRotX() { return rotateX; }
    static float GetCurrentRotY();
    static float GetCurrentRotZ() { return rotateZ; }
};