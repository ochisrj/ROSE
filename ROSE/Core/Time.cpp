#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Time.h"

float Time::s_DeltaTime = 0.0f;
float Time::s_LastTime = 0.0f;
float Time::s_Time = 0.0f;
float Time::s_FixedDeltaTime = 1.0f / 60.0f;
float Time::s_FixedAccumulator = 0.0f;
int Time::s_FrameCount = 0;
float Time::s_FPS = 0.0f;
float Time::s_FPSUpdateTimer = 0.0f;

void Time::Init() {
    s_LastTime = static_cast<float>(glfwGetTime());
    s_Time = 0.0f;
    s_FrameCount = 0;
    s_FPS = 0.0f;
    s_FixedAccumulator = 0.0f;
}

void Time::Update() {
    float currentTime = static_cast<float>(glfwGetTime());
    s_DeltaTime = currentTime - s_LastTime;
    s_LastTime = currentTime;
    s_Time += s_DeltaTime;

    s_FixedAccumulator += s_DeltaTime;

    s_FrameCount++;
    s_FPSUpdateTimer += s_DeltaTime;
    if (s_FPSUpdateTimer >= 1.0f) {
        s_FPS = static_cast<float>(s_FrameCount) / s_FPSUpdateTimer;
        s_FrameCount = 0;
        s_FPSUpdateTimer = 0.0f;
    }
}

bool Time::ShouldFixedUpdate() {
    if (s_FixedAccumulator >= s_FixedDeltaTime) {
        s_FixedAccumulator -= s_FixedDeltaTime;
        return true;
    }
    return false;
}
