#ifndef ENGINE_H
#define ENGINE_H

#include "../Render/Renderer.h"

struct GLFWwindow;

class Engine {
public:
    static Engine& Instance();

    bool Init(int width = 1920, int height = 1080, const char* title = "ROSE Engine");
    void Run();
    void Shutdown();

    GLFWwindow* GetWindow() const { return m_Window; }
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
    bool IsRunning() const { return m_Running; }

private:
    Engine() = default;

    void InitRenderPipeline();
    void ProcessInput();
    void Update();
    void FixedUpdate();
    void Render();
    void RenderImGui();

    GLFWwindow* m_Window = nullptr;
    int m_Width = 1920;
    int m_Height = 1080;
    bool m_Running = false;
};

#endif
