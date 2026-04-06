#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include "implot3d.h"
#include "implot_internal.h"
#include "implot3d_internal.h"

#include "tools_menu.h"

bool ToolMenu::PerformaceHUD = false;
bool ToolMenu::ScriptEditor = false;

void ToolMenu::DrawMenu()
{
	if (ImGui::BeginMenu("Tools"))
	{
		if(ImGui::MenuItem("Performace",NULL,&PerformaceHUD)){}
        if(ImGui::MenuItem("ScriptEditor",NULL,&ScriptEditor)){}
		ImGui::EndMenu();
	}
}

void ToolMenu::DrawWindow()
{
    if (!PerformaceHUD) return;

    // Ring-buffer for the last 120 FPS samples
    static float  fps_history[120] = {};
    static int    fps_offset = 0;
    static double fps_refresh_time = 0.0;

    double now = ImGui::GetTime();
    // Sample up to 60 times per second so the graph scrolls smoothly
    if (now - fps_refresh_time >= 1.0 / 60.0)
    {
        fps_history[fps_offset] = ImGui::GetIO().Framerate;
        fps_offset = (fps_offset + 1) % IM_ARRAYSIZE(fps_history);
        fps_refresh_time = now;
    }

    // Ring-buffer for frame time (ms) — same approach
    static float  ms_history[120] = {};
    static int    ms_offset = 0;
    {
        static double ms_refresh_time = 0.0;
        if (now - ms_refresh_time >= 1.0 / 60.0)
        {
            float framerate = ImGui::GetIO().Framerate;
            ms_history[ms_offset] = (framerate > 0.f) ? 1000.f / framerate : 0.f;
            ms_offset = (ms_offset + 1) % IM_ARRAYSIZE(ms_history);
            ms_refresh_time = now;
        }
    }

    ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.45f);
    ImGui::SetNextWindowSize(ImVec2(320, 0), ImGuiCond_Always);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_AlwaysAutoResize;

    ImGui::Begin("FPS window", &PerformaceHUD, flags);

    float currentFPS = ImGui::GetIO().Framerate;
    float currentMS = (currentFPS > 0.f) ? 1000.f / currentFPS : 0.f;

    // ── FPS coloured text ──────────────────────────────────────────────────
    ImVec4 fpsColor = (currentFPS >= 55.f) ? ImVec4(0, 1, 0, 1)
        : (currentFPS >= 30.f) ? ImVec4(1, 1, 0, 1)
        : ImVec4(1, 0, 0, 1);
    ImGui::TextColored(fpsColor, "FPS: %.1f", currentFPS);
    ImGui::SameLine();
    ImGui::TextDisabled("(%.2f ms)", currentMS);

    // ── FPS PlotLines — the graph follows the real framerate ───────────────
    ImGui::SeparatorText("FPS History");
    {
        char overlay[32];
        snprintf(overlay, sizeof(overlay), "%.0f fps", currentFPS);
        ImGui::PlotLines("##fps",
            fps_history, IM_ARRAYSIZE(fps_history), fps_offset,
            overlay,
            0.0f, 200.0f,          // y-axis: 0 – 200 fps
            ImVec2(-1, 70));
    }

    // ── Frame-time PlotLines ───────────────────────────────────────────────
    ImGui::SeparatorText("Frame Time (ms)");
    {
        char overlay[32];
        snprintf(overlay, sizeof(overlay), "%.2f ms", currentMS);
        ImGui::PlotLines("##ms",
            ms_history, IM_ARRAYSIZE(ms_history), ms_offset,
            overlay,
            0.0f, 50.0f,           // y-axis: 0 – 50 ms
            ImVec2(-1, 70));
    }

    // ── Target budget indicator ────────────────────────────────────────────
    ImGui::Spacing();
    ImGui::ProgressBar(currentFPS / 60.f, ImVec2(-1, 8), "");
    ImGui::TextDisabled("Budget vs 60 fps target");

    ImGui::End();
	
}
	
