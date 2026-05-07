#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Engine.h"
#include "Log.h"
#include "Time.h"
#include "Event.h"
#include "../Scene/SceneManager.h"
#include "../Resource/ResourceManager.h"
#include "../ECS/System.h"
#include "../ECS/Entity.h"
#include "../menubar.h"

#include "../Render/SpriteBatch.h"
#include "../Render/SpriteShader.h"
#include "../Render/OrthoCamera.h"
#include "../Render/PostProcess.h"
#include "../Render/Light2D.h"
#include "../Render/ParticleSystem.h"
#include "../Render/TilemapRenderer.h"
#include "../Render/Renderer.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include "implot_internal.h"
#include "implot3d.h"
#include "implot3d_internal.h"

Engine& Engine::Instance() {
    static Engine instance;
    return instance;
}

bool Engine::Init(int width, int height, const char* title) {
    m_Width = width;
    m_Height = height;

    Log::Init();
    ROSE_LOG_INFO("Initializing ROSE Engine...");

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(m_Width, m_Height, title, NULL, NULL);
    if (!m_Window) {
        ROSE_LOG_FATAL("Failed to create GLFW window");
        return false;
    }

    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(0);
    gladLoadGL();
    glViewport(0, 0, m_Width, m_Height);

    glEnable(GL_DEPTH_TEST);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImPlot3D::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    Time::Init();
    InitRenderPipeline();
    m_Running = true;

    ROSE_LOG_INFO("Engine initialized successfully (%dx%d)", m_Width, m_Height);
    return true;
}

void Engine::InitRenderPipeline() {
    Renderer::Instance().Init();
    SpriteShader::Instance().Init();
    SpriteBatch::Instance().Init();
    PostProcess::Instance().Init(m_Width, m_Height);
    LightManager::Instance().Init();
    ParticleSystem::Instance().Init();

    OrthoCamera::Instance().SetOrthoBounds((float)m_Width, (float)m_Height);

    ROSE_LOG_INFO("Render pipeline initialized");
}

void Engine::Run() {
    ROSE_LOG_INFO("Starting engine loop...");
    while (m_Running && !glfwWindowShouldClose(m_Window)) {
        Time::Update();
        float dt = Time::GetDeltaTime();

        ProcessInput();
        Update();

        while (Time::ShouldFixedUpdate()) {
            FixedUpdate();
        }

        Render();
        RenderImGui();

        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }
    ROSE_LOG_INFO("Engine loop ended");
}

void Engine::ProcessInput() {
    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        m_Running = false;
    }
}

void Engine::Update() {
    SceneManager::Instance().Update(Time::GetDeltaTime());
    SystemRegistry::Instance().UpdateAll(Time::GetDeltaTime());
}

void Engine::FixedUpdate() {
    SceneManager::Instance().FixedUpdate(Time::GetFixedDeltaTime());
    SystemRegistry::Instance().FixedUpdateAll(Time::GetFixedDeltaTime());
}

void Engine::Render() {
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Renderer::Instance().BeginFrame();

    PostProcess::Instance().BeginRender();

    SpriteShader::Instance().UseShader(SpriteShaderType::Default2D);
    SpriteShader::Instance().SetProjection(OrthoCamera::Instance().GetProjectionMatrix());
    SpriteShader::Instance().SetView(OrthoCamera::Instance().GetViewMatrix());

    SpriteBatch::Instance().Begin();

    SceneManager::Instance().Render();
    SystemRegistry::Instance().RenderAll();
    ParticleSystem::Instance().Update(Time::GetDeltaTime());
    ParticleSystem::Instance().Render();

    SpriteBatch::Instance().End();

    PostProcess::Instance().EndRender();
    PostProcess::Instance().ApplyEffects();

    Renderer::Instance().EndFrame();
}

void Engine::RenderImGui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    MenuBar::Draw();

    if (ImGui::Begin("Engine Stats")) {
        ImGui::Text("FPS: %.1f", Time::GetFPS());
        ImGui::Text("Frame Time: %.3f ms", Time::GetDeltaTime() * 1000.0f);
        ImGui::Text("Frame: %d", Time::GetFrameCount());
        ImGui::Text("Entities: %zu", EntityRegistry::Instance().GetEntityCount());

        Scene* active = SceneManager::Instance().GetActiveScene();
        if (active) {
            ImGui::Text("Active Scene: %s", active->GetName().c_str());
            ImGui::Text("Scene Entities: %zu", active->GetEntities().size());
        }

        ImGui::Separator();
        ImGui::Text("Render: %u sprites, %u draw calls",
                    SpriteBatch::Instance().GetSpriteCount(),
                    SpriteBatch::Instance().GetDrawCalls());

        ImGui::Text("Lights: %d/%d",
                    (int)LightManager::Instance().GetActiveLights().size(),
                    LightManager::Instance().GetMaxLights());

        ImGui::Text("Particles: %d",
                    ParticleSystem::Instance().GetActiveEmitters());
    }
    ImGui::End();

    SceneManager::Instance().EditorGUI();
    SystemRegistry::Instance().EditorGUIAll();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Engine::Shutdown() {
    ROSE_LOG_INFO("Shutting down ROSE Engine...");

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot3D::DestroyContext();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    Event::Clear();
    ResourceManager::Instance().Clear();

    glfwDestroyWindow(m_Window);
    glfwTerminate();

    Log::Shutdown();
    ROSE_LOG_INFO("Shutdown complete");
}
