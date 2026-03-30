#include "debugtool.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include "implot.h"
#include "implot_internal.h"

#include "implot3d.h"
#include "implot3d_internal.h"

#include <iostream>
#include <cmath>
#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <vector>
#include <deque>
#include <ctime>
#include <chrono>
#include <algorithm>

// ─────────────────────────────────────────────────────────────────────────────
//  CONSTRUCTOR
// ─────────────────────────────────────────────────────────────────────────────
debugtool::debugtool()
{
}

// ─────────────────────────────────────────────────────────────────────────────
//  MAIN DRAW DISPATCH
// ─────────────────────────────────────────────────────────────────────────────
void debugtool::DrawUI(ImGuiIO& io)
{
    DrawMenuBar();

    // Debug / demo windows
    DrawDemo();
    DrawDemoPlot();
    DrawDemoPlot3D();
    DrawTextFormat();

    // Tools
    DrawImageViewer();
    DrawCalculator();

    // Panels
    DrawPerformanceHUD();
    DrawAssetBrowser();
    DrawConsoleLog();
    DrawPreferences();
}

// ─────────────────────────────────────────────────────────────────────────────
//  MENU BAR
// ─────────────────────────────────────────────────────────────────────────────
void debugtool::DrawMenuBar()
{
    static bool show_quit_confirm = false;

    if (ImGui::BeginMainMenuBar())
    {
        // ── File ──────────────────────────────────────────────────────────────
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save", "Ctrl+S")) {}
            if (ImGui::MenuItem("Save As", "Ctrl+Shift+S")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Open File", "Ctrl+O")) {}
            if (ImGui::MenuItem("Open Folder")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Preferences", NULL, &preferences)) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Quit", "Alt+F4")) { show_quit_confirm = true; }
            ImGui::EndMenu();
        }

        // ── View ──────────────────────────────────────────────────────────────
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::BeginMenu("Debug Windows"))
            {
                if (ImGui::MenuItem("Demo", "", &demo_window)) {}
                if (ImGui::MenuItem("Demo Plot", "", &demo_plot)) {}
                if (ImGui::MenuItem("Demo Plot3D", "", &demo_plot3D)) {}
                if (ImGui::MenuItem("Text Demo", "", &text_formatting)) {}
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        // ── Tools ─────────────────────────────────────────────────────────────
        if (ImGui::BeginMenu("Tools"))
        {
            if (ImGui::MenuItem("Image Viewer", NULL, &image_viewer)) {}
            if (ImGui::MenuItem("Calculator", NULL, &calculator)) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Asset Browser", NULL, &asset_browser)) {}
            if (ImGui::MenuItem("Console / Log", NULL, &console_log)) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Performance HUD", NULL, &performance_hud)) {}
            ImGui::EndMenu();
        }

        // ── right side: FPS readout ───────────────────────────────────────────
        float fps = ImGui::GetIO().Framerate;
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 220);
        ImGui::Text("FPS: %.1f | ROSE Engine", fps);

        ImGui::EndMainMenuBar();
    }

    // ── Quit confirmation modal ───────────────────────────────────────────────
    if (show_quit_confirm)
    {
        ImGui::OpenPopup("Quit ROSE Engine?");
        show_quit_confirm = false;
    }
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Quit ROSE Engine?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Are you sure you want to quit?");
        ImGui::Text("Any unsaved work will be lost.");
        ImGui::Separator();
        ImGui::Spacing();
        if (ImGui::Button("Yes, Quit", ImVec2(120, 0)))
        {
            // Signal GLFW to close the window
            // The main loop checks glfwWindowShouldClose, so post a close event.
            // Because we don't hold the GLFWwindow* here, we use the standard
            // ImGui backend trick: request termination via exit().
            // Replace with glfwSetWindowShouldClose(window, true) if you pass the pointer.
            exit(0);
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  DEMO WINDOWS
// ─────────────────────────────────────────────────────────────────────────────
void debugtool::DrawDemo()
{
    if (demo_window)
        ImGui::ShowDemoWindow(&demo_window);
}

void debugtool::DrawDemoPlot()
{
    if (demo_plot)
        ImPlot::ShowDemoWindow(&demo_plot);
}

void debugtool::DrawDemoPlot3D()
{
    if (demo_plot3D)
        ImPlot3D::ShowDemoWindow(&demo_plot3D);
}

// ─────────────────────────────────────────────────────────────────────────────
//  PERFORMANCE HUD  — live ring-buffer graphs + CPU/GPU frame stats
// ─────────────────────────────────────────────────────────────────────────────
void debugtool::DrawPerformanceHUD()
{
    if (!performance_hud) return;

    static float  fps_history[120] = {};
    static float  ms_history[120] = {};
    static int    fps_offset = 0;
    static double last_sample = 0.0;

    // Throttled sampling (60 Hz)
    double now = ImGui::GetTime();
    if (now - last_sample >= 1.0 / 60.0)
    {
        float fps = ImGui::GetIO().Framerate;
        float ms = (fps > 0.f) ? 1000.f / fps : 0.f;
        fps_history[fps_offset] = fps;
        ms_history[fps_offset] = ms;
        fps_offset = (fps_offset + 1) % IM_ARRAYSIZE(fps_history);
        last_sample = now;
    }

    // Rolling min/max for FPS
    float fps_min = 9999.f, fps_max = 0.f;
    for (float v : fps_history) { fps_min = ImMin(fps_min, v); fps_max = ImMax(fps_max, v); }

    ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.80f);
    ImGui::SetNextWindowSize(ImVec2(340, 0), ImGuiCond_Always);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_AlwaysAutoResize;

    ImGui::Begin("##PerfHUD", &performance_hud, flags);

    float currentFPS = ImGui::GetIO().Framerate;
    float currentMS = (currentFPS > 0.f) ? 1000.f / currentFPS : 0.f;

    // ── Title row ─────────────────────────────────────────────────────────
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.9f, 1.0f, 1.f));
    ImGui::Text("  PERFORMANCE HUD");
    ImGui::PopStyleColor();
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 16.f);
    if (ImGui::SmallButton("X")) { performance_hud = false; }
    ImGui::Separator();

    // ── FPS coloured badge ────────────────────────────────────────────────
    ImVec4 fpsColor = (currentFPS >= 55.f) ? ImVec4(0.2f, 1.0f, 0.3f, 1.f)
        : (currentFPS >= 30.f) ? ImVec4(1.0f, 0.85f, 0.1f, 1.f)
        : ImVec4(1.0f, 0.25f, 0.2f, 1.f);
    ImGui::TextColored(fpsColor, "FPS  %.1f", currentFPS);
    ImGui::SameLine(120);
    ImGui::TextDisabled("%.3f ms", currentMS);
    ImGui::SameLine(220);
    ImGui::TextDisabled("min %.0f  max %.0f", fps_min, fps_max);

    // ── FPS PlotLines ─────────────────────────────────────────────────────
    ImGui::SeparatorText("FPS History (120 samples)");
    {
        char overlay[32];
        snprintf(overlay, sizeof(overlay), "%.0f fps", currentFPS);
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.2f, 0.9f, 0.4f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_PlotLinesHovered, ImVec4(1.0f, 1.0f, 0.3f, 1.f));
        ImGui::PlotLines("##fps",
            fps_history, IM_ARRAYSIZE(fps_history), fps_offset,
            overlay, 0.0f, 200.0f, ImVec2(-1, 70));
        ImGui::PopStyleColor(2);
    }

    // ── Frame-time PlotLines ──────────────────────────────────────────────
    ImGui::SeparatorText("Frame Time (ms)");
    {
        char overlay[32];
        snprintf(overlay, sizeof(overlay), "%.2f ms", currentMS);
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(1.0f, 0.6f, 0.1f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_PlotLinesHovered, ImVec4(1.0f, 1.0f, 0.3f, 1.f));
        ImGui::PlotLines("##ms",
            ms_history, IM_ARRAYSIZE(ms_history), fps_offset,
            overlay, 0.0f, 50.0f, ImVec2(-1, 70));
        ImGui::PopStyleColor(2);
    }

    // ── Budget bar ────────────────────────────────────────────────────────
    ImGui::Spacing();
    float budget = ImMin(currentFPS / 60.f, 1.f);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram,
        budget > 0.9f ? ImVec4(0.2f, 1.f, 0.3f, 1.f) :
        budget > 0.5f ? ImVec4(1.f, 0.85f, 0.1f, 1.f) :
        ImVec4(1.f, 0.25f, 0.2f, 1.f));
    ImGui::ProgressBar(budget, ImVec2(-1, 10), "");
    ImGui::PopStyleColor();
    ImGui::TextDisabled("  60 fps budget  (%.0f%%)", budget * 100.f);

    // ── Extra stats ───────────────────────────────────────────────────────
    ImGui::Separator();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::TextDisabled("Vertices: %d   Indices: %d",
        io.MetricsRenderVertices, io.MetricsRenderIndices);
    ImGui::TextDisabled("Active Windows: %d",
        io.MetricsActiveWindows);

    ImGui::End();
}

// ─────────────────────────────────────────────────────────────────────────────
//  TEXT FORMATTING SHOWCASE
// ─────────────────────────────────────────────────────────────────────────────
void debugtool::DrawTextFormat()
{
    if (!text_formatting) return;

    ImGui::SetNextWindowSize(ImVec2(620, 720), ImGuiCond_FirstUseEver);
    ImGui::Begin("ImGui Feature Showcase", &text_formatting);

    if (ImGui::CollapsingHeader("Text Styles", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Normal text");
        ImGui::TextDisabled("Disabled / dimmed text");
        ImGui::TextWrapped("Wrapped: Lorem ipsum dolor sit amet, consectetur "
            "adipiscing elit. Sed do eiusmod tempor incididunt ut labore.");
        ImGui::LabelText("Label", "Value next to label");
        ImGui::BulletText("Bullet item A");
        ImGui::BulletText("Bullet item B");
        ImGui::SeparatorText("Coloured Text");
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Red");   ImGui::SameLine();
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Green"); ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.3f, 0.7f, 1, 1), "Blue"); ImGui::SameLine();
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Yellow");
    }

    if (ImGui::CollapsingHeader("Buttons"))
    {
        static int btn_clicks = 0;
        if (ImGui::Button("Normal Button"))    btn_clicks++;
        ImGui::SameLine();
        if (ImGui::SmallButton("Small"))       btn_clicks++;
        ImGui::SameLine();
        ImGui::Text("Clicks: %d", btn_clicks);
        ImGui::Button("Wide Button", ImVec2(-1, 0));
    }

    if (ImGui::CollapsingHeader("Sliders & Drag"))
    {
        static float f1 = 0.5f;
        static int   i1 = 42;
        ImGui::SliderFloat("Float", &f1, 0.f, 1.f);
        ImGui::SliderInt("Integer", &i1, 0, 100);
        static float drag_f = 0.f;
        ImGui::DragFloat("Drag Float", &drag_f, 0.01f, -10.f, 10.f, "%.3f");
    }

    if (ImGui::CollapsingHeader("Plot Preview"))
    {
        static float sine[128];
        static bool  init = false;
        if (!init) { for (int i = 0; i < 128; i++) sine[i] = sinf((float)i * 0.1f); init = true; }
        ImGui::PlotLines("Sine Wave", sine, 128, 0, NULL, -1.5f, 1.5f, ImVec2(-1, 60));
    }

    ImGui::End();
}

// ─────────────────────────────────────────────────────────────────────────────
//  IMAGE VIEWER  — drag-and-drop path entry, zoom/pan, channel toggles
// ─────────────────────────────────────────────────────────────────────────────
void debugtool::DrawImageViewer()
{
    if (!image_viewer) return;

    // ── persistent state ───────────────────────────────────────────────────
    static char   path_buf[512] = "";
    static float  zoom = 1.0f;
    static ImVec2 pan = { 0.f, 0.f };
    static bool   show_r = true;
    static bool   show_g = true;
    static bool   show_b = true;
    static bool   show_a = true;
    static bool   fit_on_load = true;
    static bool   dragging = false;
    static ImVec2 drag_start = { 0.f, 0.f };
    static ImVec2 pan_start = { 0.f, 0.f };

    // ── placeholder "checkerboard" texture size ────────────────────────────
    static int    img_w = 0;
    static int    img_h = 0;
    // In a real implementation you would store GLuint tex_id here.
    // For now we keep it as a stub that shows the full UI ready for wiring.
    // static GLuint tex_id = 0;

    ImGui::SetNextWindowSize(ImVec2(680, 520), ImGuiCond_FirstUseEver);
    ImGui::Begin("Image Viewer", &image_viewer);

    // ── Toolbar ────────────────────────────────────────────────────────────
    ImGui::PushItemWidth(380);
    bool load_pressed = false;
    if (ImGui::InputText("##imgpath", path_buf, sizeof(path_buf),
        ImGuiInputTextFlags_EnterReturnsTrue))
        load_pressed = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Button("Load") || load_pressed)
    {
        // ── Wire texture loading here, e.g.: ──────────────────────────────
        // if (tex_id) glDeleteTextures(1, &tex_id);
        // LoadTextureFromFile(path_buf, &tex_id, &img_w, &img_h);
        // if (fit_on_load && img_w > 0) { zoom = 1.f; pan = {0,0}; }
        (void)fit_on_load; // suppress unused warning until wired
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) { zoom = 1.f; pan = { 0.f, 0.f }; }
    ImGui::SameLine();
    ImGui::TextDisabled("Zoom: %.0f%%", zoom * 100.f);

    // ── Channel toggles ────────────────────────────────────────────────────
    ImGui::SameLine(0, 16);
    ImGui::TextDisabled("Ch:");
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, show_r ? ImVec4(0.7f, 0.1f, 0.1f, 1) : ImVec4(0.3f, 0.1f, 0.1f, 1));
    if (ImGui::SmallButton("R")) show_r = !show_r; ImGui::PopStyleColor(); ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, show_g ? ImVec4(0.1f, 0.7f, 0.1f, 1) : ImVec4(0.1f, 0.3f, 0.1f, 1));
    if (ImGui::SmallButton("G")) show_g = !show_g; ImGui::PopStyleColor(); ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, show_b ? ImVec4(0.1f, 0.3f, 0.9f, 1) : ImVec4(0.1f, 0.1f, 0.3f, 1));
    if (ImGui::SmallButton("B")) show_b = !show_b; ImGui::PopStyleColor(); ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, show_a ? ImVec4(0.5f, 0.5f, 0.5f, 1) : ImVec4(0.2f, 0.2f, 0.2f, 1));
    if (ImGui::SmallButton("A")) show_a = !show_a; ImGui::PopStyleColor();
    ImGui::SameLine(0, 16);
    ImGui::Checkbox("Fit on load", &fit_on_load);

    ImGui::Separator();

    // ── Canvas region ──────────────────────────────────────────────────────
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    if (canvas_size.x < 10) canvas_size.x = 10;
    if (canvas_size.y < 10) canvas_size.y = 10;

    ImDrawList* draw = ImGui::GetWindowDrawList();

    // Checkerboard background
    {
        const int cell = 16;
        const ImU32 c1 = IM_COL32(80, 80, 80, 255);
        const ImU32 c2 = IM_COL32(60, 60, 60, 255);
        for (float y = 0; y < canvas_size.y; y += cell)
            for (float x = 0; x < canvas_size.x; x += cell)
            {
                int cx = (int)(x / cell), cy = (int)(y / cell);
                ImU32 col = ((cx + cy) % 2 == 0) ? c1 : c2;
                draw->AddRectFilled(
                    { canvas_pos.x + x,               canvas_pos.y + y },
                    { canvas_pos.x + x + (float)cell, canvas_pos.y + y + (float)cell },
                    col);
            }
    }

    // ── Image draw (stub) ──────────────────────────────────────────────────
    // When tex_id is valid, draw it with zoom and pan:
    //  ImVec2 img_tl = { canvas_pos.x + pan.x + (canvas_size.x - img_w * zoom) * 0.5f,
    //                    canvas_pos.y + pan.y + (canvas_size.y - img_h * zoom) * 0.5f };
    //  ImVec2 img_br = { img_tl.x + img_w * zoom, img_tl.y + img_h * zoom };
    //  draw->AddImage((ImTextureID)(intptr_t)tex_id, img_tl, img_br);

    // Placeholder text centred in the canvas
    const char* hint = (path_buf[0] == '\0')
        ? "Enter image path above and click  Load"
        : "Texture loading not yet wired — add LoadTextureFromFile() here";
    ImVec2 ts = ImGui::CalcTextSize(hint);
    draw->AddText(
        { canvas_pos.x + (canvas_size.x - ts.x) * 0.5f,
          canvas_pos.y + (canvas_size.y - ts.y) * 0.5f },
        IM_COL32(180, 180, 180, 160), hint);

    // ── Invisible interaction rect (zoom + pan) ────────────────────────────
    ImGui::InvisibleButton("##canvas", canvas_size);
    if (ImGui::IsItemHovered())
    {
        // Scroll-wheel zoom
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0.f) zoom = ImClamp(zoom * (1.f + wheel * 0.12f), 0.05f, 32.f);
    }
    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        if (!dragging)
        {
            dragging = true;
            drag_start = ImGui::GetMousePos();
            pan_start = pan;
        }
        ImVec2 mp = ImGui::GetMousePos();
        pan = { pan_start.x + (mp.x - drag_start.x),
                pan_start.y + (mp.y - drag_start.y) };
    }
    else { dragging = false; }

    // ── Status bar ────────────────────────────────────────────────────────
    ImGui::SetCursorScreenPos({ canvas_pos.x, canvas_pos.y + canvas_size.y - 20.f });
    if (img_w > 0 && img_h > 0)
        ImGui::TextDisabled("  %d x %d px  |  zoom %.0f%%  |  pan %.0f,%.0f",
            img_w, img_h, zoom * 100.f, pan.x, pan.y);
    else
        ImGui::TextDisabled("  No image loaded  |  Scroll to zoom  |  Drag to pan");

    ImGui::End();
}

// ─────────────────────────────────────────────────────────────────────────────
//  CALCULATOR  (unchanged from original)
// ─────────────────────────────────────────────────────────────────────────────
void debugtool::DrawCalculator()
{
    if (!calculator) return;

    static double displayVal = 0.0;
    static double storedVal = 0.0;
    static char   op = 0;
    static bool   newNumber = true;
    static bool   justEvaled = false;
    static char   displayBuf[64] = "0";
    static char   exprBuf[64] = "";

    auto refreshBuf = [&]() {
        if (displayVal == (long long)displayVal && displayVal < 1e15 && displayVal > -1e15)
            snprintf(displayBuf, sizeof(displayBuf), "%.0f", displayVal);
        else
            snprintf(displayBuf, sizeof(displayBuf), "%.10g", displayVal);
        };

    auto digitPressed = [&](int d) {
        if (justEvaled) { displayVal = 0; newNumber = true; justEvaled = false; }
        if (newNumber) { displayVal = (double)d; newNumber = false; }
        else { displayVal = displayVal * 10.0 + d; }
        refreshBuf();
        };

    auto compute = [&](char o, double lhs, double rhs) -> double {
        switch (o) {
        case '+': return lhs + rhs;
        case '-': return lhs - rhs;
        case '*': return lhs * rhs;
        case '/': return (rhs != 0) ? lhs / rhs : 0.0;
        case 'p': return pow(lhs, rhs);
        case '%': return lhs * (rhs / 100.0);
        }
        return rhs;
        };

    auto opPressed = [&](char o) {
        if (op && !newNumber) { displayVal = compute(op, storedVal, displayVal); refreshBuf(); }
        storedVal = displayVal; op = o; newNumber = true; justEvaled = false;
        char opCh = (o == 'p') ? '^' : o;
        snprintf(exprBuf, sizeof(exprBuf), "%s %c", displayBuf, opCh);
        };

    auto evalPressed = [&]() {
        if (op == 0) return;
        displayVal = compute(op, storedVal, displayVal);
        op = 0; justEvaled = true; newNumber = true;
        refreshBuf();
        snprintf(exprBuf, sizeof(exprBuf), "");
        };

    auto clearPressed = [&]() {
        displayVal = storedVal = 0; op = 0; newNumber = true; justEvaled = false;
        snprintf(displayBuf, sizeof(displayBuf), "0");
        snprintf(exprBuf, sizeof(exprBuf), "");
        };

    auto dotPressed = [&]() {
        if (newNumber) { snprintf(displayBuf, sizeof(displayBuf), "0."); displayVal = 0; newNumber = false; }
        else if (!strchr(displayBuf, '.')) {
            size_t len = strlen(displayBuf);
            if (len + 2 < sizeof(displayBuf)) { displayBuf[len] = '.'; displayBuf[len + 1] = '\0'; }
        }
        };

    auto backPressed = [&]() {
        if (justEvaled) return;
        size_t len = strlen(displayBuf);
        if (len > 1) { displayBuf[len - 1] = '\0'; displayVal = atof(displayBuf); }
        else { snprintf(displayBuf, sizeof(displayBuf), "0"); displayVal = 0; }
        };

    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantTextInput) {
        for (int d = 0; d <= 9; d++) {
            if (ImGui::IsKeyPressed((ImGuiKey)((int)ImGuiKey_Keypad0 + d), false) ||
                ImGui::IsKeyPressed((ImGuiKey)((int)ImGuiKey_0 + d), false))
                digitPressed(d);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_KeypadAdd, false))      opPressed('+');
        if (ImGui::IsKeyPressed(ImGuiKey_KeypadSubtract, false)) opPressed('-');
        if (ImGui::IsKeyPressed(ImGuiKey_KeypadMultiply, false)) opPressed('*');
        if (ImGui::IsKeyPressed(ImGuiKey_KeypadDivide, false))   opPressed('/');
        if (ImGui::IsKeyPressed(ImGuiKey_KeypadEnter, false) ||
            ImGui::IsKeyPressed(ImGuiKey_Enter, false))          evalPressed();
        if (ImGui::IsKeyPressed(ImGuiKey_Backspace, false))      backPressed();
        if (ImGui::IsKeyPressed(ImGuiKey_Escape, false))         clearPressed();
        if (ImGui::IsKeyPressed(ImGuiKey_KeypadDecimal, false) ||
            ImGui::IsKeyPressed(ImGuiKey_Period, false))         dotPressed();
    }

    ImGui::SetNextWindowSize(ImVec2(296, 460), ImGuiCond_Always);
    ImGui::Begin("Calculator", &calculator, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

    float panelW = ImGui::GetContentRegionAvail().x;
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.08f, 0.10f, 1.f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 4.f);
    ImGui::BeginChild("##disp", ImVec2(panelW, 64), false);
    if (exprBuf[0]) {
        float hw = ImGui::CalcTextSize(exprBuf).x;
        ImGui::SetCursorPos(ImVec2(panelW - hw - 6, 4));
        ImGui::TextDisabled("%s", exprBuf);
    }
    float nw = ImGui::CalcTextSize(displayBuf).x;
    ImGui::SetCursorPos(ImVec2(panelW - nw - 6, 36));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
    ImGui::TextUnformatted(displayBuf);
    ImGui::PopStyleColor();
    ImGui::EndChild();
    ImGui::PopStyleVar(); ImGui::PopStyleColor();
    ImGui::Spacing();

    const float spacing = 4.f;
    const float btnW = (panelW - spacing * 3.f) / 4.f;
    const float btnH = 52.f;
    const ImVec2 B(btnW, btnH);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing, spacing));

    const ImVec4 C_DIGIT_N(0.22f, 0.22f, 0.26f, 1.f), C_DIGIT_H(0.30f, 0.30f, 0.36f, 1.f), C_DIGIT_A(0.15f, 0.15f, 0.18f, 1.f);
    const ImVec4 C_OP_N(0.75f, 0.48f, 0.06f, 1.f), C_OP_H(0.90f, 0.60f, 0.12f, 1.f), C_OP_A(0.58f, 0.36f, 0.02f, 1.f);
    const ImVec4 C_UTIL_N(0.28f, 0.35f, 0.50f, 1.f), C_UTIL_H(0.36f, 0.45f, 0.65f, 1.f), C_UTIL_A(0.20f, 0.26f, 0.38f, 1.f);
    const ImVec4 C_EQ_N(0.08f, 0.58f, 0.53f, 1.f), C_EQ_H(0.12f, 0.72f, 0.66f, 1.f), C_EQ_A(0.05f, 0.42f, 0.38f, 1.f);

    auto Btn = [&](const char* label, ImVec2 sz, ImVec4 cn, ImVec4 ch, ImVec4 ca)->bool {
        ImGui::PushStyleColor(ImGuiCol_Button, cn);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ch);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ca);
        bool c = ImGui::Button(label, sz);
        ImGui::PopStyleColor(3); return c;
        };
    auto BtnD = [&](const char* l)->bool {return Btn(l, B, C_DIGIT_N, C_DIGIT_H, C_DIGIT_A); };
    auto BtnO = [&](const char* l)->bool {return Btn(l, B, C_OP_N, C_OP_H, C_OP_A); };
    auto BtnU = [&](const char* l)->bool {return Btn(l, B, C_UTIL_N, C_UTIL_H, C_UTIL_A); };
    auto BtnE = [&](const char* l)->bool {return Btn(l, B, C_EQ_N, C_EQ_H, C_EQ_A); };

    if (BtnU("C"))   clearPressed();                                    ImGui::SameLine();
    if (BtnU("CE")) { displayVal = 0; newNumber = true; refreshBuf(); }         ImGui::SameLine();
    if (BtnU("+/-")) { displayVal = -displayVal; refreshBuf(); }             ImGui::SameLine();
    if (BtnU("<X"))  backPressed();

    if (BtnO("sqrt")) { displayVal = sqrt(fabs(displayVal)); refreshBuf(); newNumber = true; }ImGui::SameLine();
    if (BtnO("x^y")) opPressed('p'); ImGui::SameLine();
    if (BtnO("%"))   opPressed('%'); ImGui::SameLine();
    if (BtnO("/"))   opPressed('/');

    if (BtnD("7"))digitPressed(7); ImGui::SameLine();
    if (BtnD("8"))digitPressed(8); ImGui::SameLine();
    if (BtnD("9"))digitPressed(9); ImGui::SameLine();
    if (BtnO("*"))opPressed('*');

    if (BtnD("4"))digitPressed(4); ImGui::SameLine();
    if (BtnD("5"))digitPressed(5); ImGui::SameLine();
    if (BtnD("6"))digitPressed(6); ImGui::SameLine();
    if (BtnO("-"))opPressed('-');

    if (BtnD("1"))digitPressed(1); ImGui::SameLine();
    if (BtnD("2"))digitPressed(2); ImGui::SameLine();
    if (BtnD("3"))digitPressed(3); ImGui::SameLine();
    if (BtnO("+"))opPressed('+');

    if (BtnD("0"))digitPressed(0); ImGui::SameLine();
    if (BtnD("."))dotPressed();   ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
    ImGui::Button("##empty", B);
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    if (BtnE("="))evalPressed();

    ImGui::PopStyleVar(2);
    ImGui::Spacing();
    ImGui::TextDisabled("Numpad / keyboard shortcuts active");
    ImGui::End();
}

// ─────────────────────────────────────────────────────────────────────────────
//  ASSET BROWSER
//  Left panel: folder tree  |  Right panel: asset grid with icon + name cards
// ─────────────────────────────────────────────────────────────────────────────
void debugtool::DrawAssetBrowser()
{
    if (!asset_browser) return;

    // ── Simulated asset data ───────────────────────────────────────────────
    struct AssetEntry { const char* name; const char* ext; const char* folder; ImVec4 col; };
    static const AssetEntry assets[] = {
        {"player_idle",  ".png",  "Textures",  {0.4f,0.7f,1.0f,1.f}},
        {"player_run",   ".png",  "Textures",  {0.4f,0.7f,1.0f,1.f}},
        {"tile_grass",   ".png",  "Textures",  {0.4f,0.7f,1.0f,1.f}},
        {"skybox_day",   ".hdr",  "Textures",  {0.9f,0.7f,0.2f,1.f}},
        {"player_mesh",  ".obj",  "Meshes",    {0.5f,1.0f,0.5f,1.f}},
        {"tree_01",      ".obj",  "Meshes",    {0.5f,1.0f,0.5f,1.f}},
        {"rock_pile",    ".fbx",  "Meshes",    {0.5f,1.0f,0.5f,1.f}},
        {"main_theme",   ".ogg",  "Audio",     {1.0f,0.5f,0.8f,1.f}},
        {"jump_sfx",     ".wav",  "Audio",     {1.0f,0.5f,0.8f,1.f}},
        {"player_mat",   ".mat",  "Materials", {1.0f,0.8f,0.3f,1.f}},
        {"terrain_mat",  ".mat",  "Materials", {1.0f,0.8f,0.3f,1.f}},
        {"game_scene",   ".scene","Scenes",    {0.8f,0.5f,1.0f,1.f}},
        {"menu_scene",   ".scene","Scenes",    {0.8f,0.5f,1.0f,1.f}},
    };
    static const char* folders[] = { "All", "Textures", "Meshes", "Audio", "Materials", "Scenes" };
    static int  selected_folder = 0;   // index into folders[]
    static int  selected_asset = -1;
    static char search_buf[64] = "";
    static float thumb_size = 64.f;

    ImGui::SetNextWindowSize(ImVec2(740, 460), ImGuiCond_FirstUseEver);
    ImGui::Begin("Asset Browser", &asset_browser);

    // ── Top toolbar ───────────────────────────────────────────────────────
    ImGui::SetNextItemWidth(220);
    ImGui::InputTextWithHint("##search", "Search assets...", search_buf, sizeof(search_buf));
    ImGui::SameLine();
    ImGui::TextDisabled("Thumb:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    ImGui::SliderFloat("##thumb", &thumb_size, 32.f, 128.f, "%.0fpx");
    ImGui::SameLine();
    if (ImGui::Button("Import")) {}
    ImGui::SameLine();
    if (ImGui::Button("Refresh")) {}
    ImGui::Separator();

    // ── Left: folder panel ────────────────────────────────────────────────
    ImGui::BeginChild("##folders", ImVec2(140, -1), true);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.85f, 1.0f, 1.f));
    ImGui::Text("  FOLDERS");
    ImGui::PopStyleColor();
    ImGui::Separator();
    for (int i = 0; i < IM_ARRAYSIZE(folders); i++)
    {
        bool sel = (selected_folder == i);
        if (ImGui::Selectable(folders[i], sel)) { selected_folder = i; selected_asset = -1; }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // ── Right: asset grid ─────────────────────────────────────────────────
    ImGui::BeginChild("##assets", ImVec2(0, -1), false);
    ImGui::Spacing();

    const char* filter_folder = folders[selected_folder];
    float avail = ImGui::GetContentRegionAvail().x;
    float cell_w = thumb_size + 16.f;
    int   cols = ImMax(1, (int)(avail / cell_w));

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.f, 8.f));

    int col_idx = 0;
    for (int i = 0; i < IM_ARRAYSIZE(assets); i++)
    {
        const AssetEntry& a = assets[i];

        // Folder filter
        if (selected_folder != 0 && strcmp(a.folder, filter_folder) != 0)
            continue;

        // Search filter
        if (search_buf[0] != '\0')
        {
            // Simple substring match (case-insensitive not needed for demo)
            if (strstr(a.name, search_buf) == nullptr &&
                strstr(a.ext, search_buf) == nullptr)
                continue;
        }

        if (col_idx > 0 && col_idx % cols != 0) ImGui::SameLine();

        // ── Thumbnail card ────────────────────────────────────────────────
        ImGui::PushID(i);
        bool selected = (selected_asset == i);

        ImVec2 card = { thumb_size + 8.f, thumb_size + 22.f };
        if (selected)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.40f, 0.65f, 1.f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.30f, 0.48f, 0.78f, 1.f));
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.14f, 0.14f, 0.16f, 1.f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.20f, 0.22f, 0.28f, 1.f));
        }
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.35f, 0.52f, 0.85f, 1.f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);

        if (ImGui::Button("##card", card)) selected_asset = i;
        bool hovered = ImGui::IsItemHovered();

        // Draw coloured icon on top of the button
        ImVec2 btn_min = ImGui::GetItemRectMin();
        ImDrawList* dl = ImGui::GetWindowDrawList();
        float icon_pad = 8.f;
        ImVec2 icon_tl = { btn_min.x + icon_pad,        btn_min.y + icon_pad };
        ImVec2 icon_br = { btn_min.x + thumb_size - 0.f, btn_min.y + thumb_size - 4.f };
        dl->AddRectFilled(icon_tl, icon_br, ImGui::ColorConvertFloat4ToU32(
            ImVec4(a.col.x * 0.4f, a.col.y * 0.4f, a.col.z * 0.4f, 1.f)), 4.f);
        // Extension badge
        ImVec2 ts = ImGui::CalcTextSize(a.ext);
        dl->AddText({ icon_tl.x + (thumb_size - icon_pad * 2 - ts.x) * 0.5f,
                      icon_tl.y + (thumb_size - icon_pad * 2 - ts.y) * 0.5f },
            ImGui::ColorConvertFloat4ToU32(a.col), a.ext);
        // Name label
        ImVec2 name_ts = ImGui::CalcTextSize(a.name);
        float  label_x = btn_min.x + (card.x - name_ts.x) * 0.5f;
        float  label_y = icon_br.y + 2.f;
        if (name_ts.x > card.x - 4.f)
        {
            // Truncate with ellipsis manually
            char trunc[32];
            snprintf(trunc, sizeof(trunc), "%.10s..", a.name);
            ImVec2 tts = ImGui::CalcTextSize(trunc);
            dl->AddText({ btn_min.x + (card.x - tts.x) * 0.5f, label_y },
                IM_COL32(210, 210, 210, 255), trunc);
        }
        else
        {
            dl->AddText({ label_x, label_y }, IM_COL32(210, 210, 210, 255), a.name);
        }

        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);

        // Tooltip on hover
        if (hovered)
        {
            ImGui::BeginTooltip();
            ImGui::TextColored(a.col, "%s%s", a.name, a.ext);
            ImGui::TextDisabled("Folder: %s", a.folder);
            ImGui::EndTooltip();
        }

        ImGui::PopID();
        col_idx++;
    }

    ImGui::PopStyleVar(); // ItemSpacing

    // ── Bottom status bar ─────────────────────────────────────────────────
    ImGui::Spacing();
    ImGui::Separator();
    if (selected_asset >= 0)
    {
        const AssetEntry& a = assets[selected_asset];
        ImGui::TextColored(a.col, "%s%s", a.name, a.ext);
        ImGui::SameLine(220);
        ImGui::TextDisabled("Folder: %s", a.folder);
    }
    else
    {
        ImGui::TextDisabled("%d assets shown", IM_ARRAYSIZE(assets));
    }

    ImGui::EndChild(); // assets
    ImGui::End();
}

// ─────────────────────────────────────────────────────────────────────────────
//  CONSOLE / LOG
//  • Multi-level filtering (Info / Warning / Error / Debug)
//  • Timestamped entries, auto-scroll, clear button, copy
//  • Sample log messages so the window isn't empty on first open
// ─────────────────────────────────────────────────────────────────────────────

// Log level enum
enum LogLevel { LOG_DEBUG = 0, LOG_INFO, LOG_WARNING, LOG_ERROR };

struct LogEntry
{
    LogLevel    level;
    std::string timestamp;
    std::string message;
};

// Global log store — use a deque so we can front-pop if it grows too large
static std::deque<LogEntry> g_log_entries;
static bool                 g_log_seeded = false;

// Public helper — call this from anywhere in the engine
void AddLog(LogLevel level, const char* fmt, ...)
{
    char buf[512];
    va_list args; va_start(args, fmt); vsnprintf(buf, sizeof(buf), fmt, args); va_end(args);

    // Timestamp from wall-clock
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    char ts[16];
    std::tm tm_info;
#if defined(_WIN32)
    localtime_s(&tm_info, &t);
#else
    localtime_r(&t, &tm_info);
#endif
    strftime(ts, sizeof(ts), "%H:%M:%S", &tm_info);

    LogEntry e; e.level = level; e.timestamp = ts; e.message = buf;
    g_log_entries.push_back(e);
    if (g_log_entries.size() > 2000) g_log_entries.pop_front();
}

void debugtool::DrawConsoleLog()
{
    if (!console_log) return;

    // Seed demo messages once
    if (!g_log_seeded)
    {
        AddLog(LOG_INFO, "ROSE Engine started");
        AddLog(LOG_INFO, "OpenGL 3.3 Core Profile initialised");
        AddLog(LOG_INFO, "ImGui %s loaded", IMGUI_VERSION);
        AddLog(LOG_DEBUG, "Shader compiled: shaderProgram = 1");
        AddLog(LOG_DEBUG, "VAO = 1  VBO = 1");
        AddLog(LOG_WARNING, "Texture loading not wired in Image Viewer");
        AddLog(LOG_WARNING, "Asset Browser running in demo mode");
        AddLog(LOG_ERROR, "Example error: file 'missing_asset.png' not found");
        AddLog(LOG_INFO, "Main loop running at target 60 fps");
        g_log_seeded = true;
    }

    // ── Filter state ──────────────────────────────────────────────────────
    static bool show_debug = true;
    static bool show_info = true;
    static bool show_warning = true;
    static bool show_error = true;
    static char search_buf[128] = "";
    static bool auto_scroll = true;
    static bool scroll_to_bottom = false;

    ImGui::SetNextWindowSize(ImVec2(760, 340), ImGuiCond_FirstUseEver);
    ImGui::Begin("Console / Log", &console_log);

    // ── Toolbar ───────────────────────────────────────────────────────────
    if (ImGui::Button("Clear")) { g_log_entries.clear(); }
    ImGui::SameLine();
    if (ImGui::Button("Copy All"))
    {
        std::string all;
        for (auto& e : g_log_entries)
            all += "[" + e.timestamp + "] " + e.message + "\n";
        ImGui::SetClipboardText(all.c_str());
    }
    ImGui::SameLine();
    ImGui::Checkbox("Auto-scroll", &auto_scroll);
    ImGui::SameLine(0, 16);
    ImGui::TextDisabled("|");
    ImGui::SameLine(0, 16);

    // Level toggles with colour
    ImGui::PushStyleColor(ImGuiCol_Button, show_debug ? ImVec4(0.3f, 0.3f, 0.5f, 1) : ImVec4(0.15f, 0.15f, 0.2f, 1));
    if (ImGui::SmallButton("DBG")) show_debug = !show_debug; ImGui::PopStyleColor(); ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, show_info ? ImVec4(0.1f, 0.5f, 0.1f, 1) : ImVec4(0.05f, 0.2f, 0.05f, 1));
    if (ImGui::SmallButton("INF")) show_info = !show_info;   ImGui::PopStyleColor(); ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, show_warning ? ImVec4(0.6f, 0.5f, 0.0f, 1) : ImVec4(0.25f, 0.2f, 0.0f, 1));
    if (ImGui::SmallButton("WRN")) show_warning = !show_warning; ImGui::PopStyleColor(); ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, show_error ? ImVec4(0.7f, 0.1f, 0.1f, 1) : ImVec4(0.28f, 0.05f, 0.05f, 1));
    if (ImGui::SmallButton("ERR")) show_error = !show_error; ImGui::PopStyleColor();

    ImGui::SameLine(0, 16);
    ImGui::SetNextItemWidth(200);
    if (ImGui::InputTextWithHint("##logsearch", "Filter...", search_buf, sizeof(search_buf)))
        scroll_to_bottom = false;

    ImGui::Separator();

    // ── Log area ──────────────────────────────────────────────────────────
    float footer_h = ImGui::GetTextLineHeightWithSpacing() + 8.f;
    ImGui::BeginChild("##logscroll", ImVec2(0, -footer_h), false, ImGuiWindowFlags_HorizontalScrollbar);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

    for (auto& e : g_log_entries)
    {
        // Level filter
        if (e.level == LOG_DEBUG && !show_debug)   continue;
        if (e.level == LOG_INFO && !show_info)    continue;
        if (e.level == LOG_WARNING && !show_warning) continue;
        if (e.level == LOG_ERROR && !show_error)   continue;
        // Search filter
        if (search_buf[0] && strstr(e.message.c_str(), search_buf) == nullptr)
            continue;

        // Colour per level
        ImVec4 col;
        const char* badge;
        switch (e.level) {
        case LOG_DEBUG:   col = { 0.55f,0.55f,0.75f,1.f }; badge = "DBG"; break;
        case LOG_INFO:    col = { 0.70f,0.90f,0.70f,1.f }; badge = "INF"; break;
        case LOG_WARNING: col = { 1.00f,0.85f,0.20f,1.f }; badge = "WRN"; break;
        case LOG_ERROR:   col = { 1.00f,0.35f,0.35f,1.f }; badge = "ERR"; break;
        default:          col = { 0.80f,0.80f,0.80f,1.f }; badge = "   ";
        }

        ImGui::TextDisabled("%s", e.timestamp.c_str());
        ImGui::SameLine();
        ImGui::TextColored(col, "[%s]", badge);
        ImGui::SameLine();
        ImGui::TextUnformatted(e.message.c_str());
    }

    ImGui::PopStyleVar();

    if (auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 4.f)
        ImGui::SetScrollHereY(1.0f);
    if (scroll_to_bottom) { ImGui::SetScrollHereY(1.0f); scroll_to_bottom = false; }

    ImGui::EndChild();

    // ── Command input ────────────────────────────────────────────────────
    ImGui::Separator();
    static char cmd_buf[256] = "";
    ImGui::SetNextItemWidth(-70.f);
    bool reclaim = false;
    if (ImGui::InputTextWithHint("##cmd", "Enter command...", cmd_buf, sizeof(cmd_buf),
        ImGuiInputTextFlags_EnterReturnsTrue))
    {
        if (cmd_buf[0])
        {
            AddLog(LOG_INFO, "> %s", cmd_buf);
            // Dispatch commands here
            if (strcmp(cmd_buf, "clear") == 0) g_log_entries.clear();
            else AddLog(LOG_WARNING, "Unknown command: '%s'", cmd_buf);
            cmd_buf[0] = '\0';
            scroll_to_bottom = true;
        }
        reclaim = true;
    }
    if (reclaim) ImGui::SetKeyboardFocusHere(-1);
    ImGui::SameLine();
    if (ImGui::Button("Send", ImVec2(-1, 0)))
    {
        if (cmd_buf[0])
        {
            AddLog(LOG_INFO, "> %s", cmd_buf);
            cmd_buf[0] = '\0';
            scroll_to_bottom = true;
        }
    }

    ImGui::End();
}

// ─────────────────────────────────────────────────────────────────────────────
//  PREFERENCES
//  Categories: Appearance | Editor | Keybindings | Build
// ─────────────────────────────────────────────────────────────────────────────
void debugtool::DrawPreferences()
{
    if (!preferences) return;

    static int    pref_tab = 0;
    static float  ui_scale = 1.0f;
    static bool   dark_theme = true;
    static bool   show_fps_always = true;
    static bool   show_grid = true;
    static ImVec4 accent_color = { 0.26f, 0.59f, 0.98f, 1.00f };
    static float  font_size = 19.f;

    static bool   autosave = true;
    static int    autosave_interval = 5;
    static bool   vsync = true;
    static int    msaa = 4;
    static char   asset_dir[256] = "./assets";
    static bool   hot_reload = true;

    static char   key_save[32] = "Ctrl+S";
    static char   key_play[32] = "F5";
    static char   key_pause[32] = "F6";
    static char   key_stop[32] = "F7";
    static char   key_console[32] = "~";

    static int    build_target = 0;  // 0=Windows 1=Linux 2=Web
    static bool   optimize = false;
    static bool   strip_debug = false;
    static char   output_dir[256] = "./build";

    ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin("Preferences", &preferences);

    // ── Left category list ────────────────────────────────────────────────
    static const char* categories[] = { "Appearance","Editor","Keybindings","Build" };
    ImGui::BeginChild("##pref_cats", ImVec2(150, -40), true);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.85f, 1.0f, 1.f));
    ImGui::Text("  SETTINGS");
    ImGui::PopStyleColor();
    ImGui::Separator();
    for (int i = 0; i < IM_ARRAYSIZE(categories); i++)
    {
        if (ImGui::Selectable(categories[i], pref_tab == i))
            pref_tab = i;
    }
    ImGui::EndChild();
    ImGui::SameLine();

    // ── Right content area ────────────────────────────────────────────────
    ImGui::BeginChild("##pref_content", ImVec2(0, -40), false);

    // ── Appearance ────────────────────────────────────────────────────────
    if (pref_tab == 0)
    {
        ImGui::SeparatorText("Appearance");
        ImGui::Checkbox("Dark Theme", &dark_theme);
        if (dark_theme) ImGui::StyleColorsDark(); else ImGui::StyleColorsLight();
        ImGui::Spacing();
        ImGui::SliderFloat("UI Scale", &ui_scale, 0.5f, 2.0f, "%.1fx");
        ImGui::SliderFloat("Font Size", &font_size, 10.f, 32.f, "%.0fpx");
        ImGui::ColorEdit4("Accent Color", (float*)&accent_color);
        ImGui::Spacing();
        ImGui::SeparatorText("HUD");
        ImGui::Checkbox("Always show FPS counter", &show_fps_always);
        ImGui::Checkbox("Show editor grid", &show_grid);
    }

    // ── Editor ────────────────────────────────────────────────────────────
    else if (pref_tab == 1)
    {
        ImGui::SeparatorText("General");
        ImGui::Checkbox("Auto-save", &autosave);
        if (autosave)
        {
            ImGui::SameLine(0, 16);
            ImGui::SetNextItemWidth(80);
            ImGui::InputInt("##asvi", &autosave_interval, 1, 5);
            ImGui::SameLine();
            ImGui::TextDisabled("minutes");
            autosave_interval = ImMax(1, autosave_interval);
        }
        ImGui::Checkbox("VSync", &vsync);
        ImGui::SliderInt("MSAA samples", &msaa, 1, 16);
        ImGui::Checkbox("Hot-reload assets on change", &hot_reload);
        ImGui::Spacing();
        ImGui::SeparatorText("Paths");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("Asset Directory", asset_dir, sizeof(asset_dir));
    }

    // ── Keybindings ───────────────────────────────────────────────────────
    else if (pref_tab == 2)
    {
        ImGui::SeparatorText("Editor Shortcuts");
        struct KBEntry { const char* action; char* buf; };
        KBEntry bindings[] = {
            {"Save",    key_save},
            {"Play",    key_play},
            {"Pause",   key_pause},
            {"Stop",    key_stop},
            {"Console", key_console},
        };
        if (ImGui::BeginTable("##kb", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 140);
            ImGui::TableSetupColumn("Binding", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();
            for (auto& b : bindings)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn(); ImGui::Text("%s", b.action);
                ImGui::TableNextColumn();
                ImGui::PushID(b.action);
                ImGui::SetNextItemWidth(-1);
                ImGui::InputText("##kb", b.buf, 32);
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
        ImGui::Spacing();
        if (ImGui::Button("Reset to Defaults"))
        {
            snprintf(key_save, sizeof(key_save), "Ctrl+S");
            snprintf(key_play, sizeof(key_play), "F5");
            snprintf(key_pause, sizeof(key_pause), "F6");
            snprintf(key_stop, sizeof(key_stop), "F7");
            snprintf(key_console, sizeof(key_console), "~");
        }
    }

    // ── Build ─────────────────────────────────────────────────────────────
    else if (pref_tab == 3)
    {
        ImGui::SeparatorText("Build Settings");
        static const char* targets[] = { "Windows x64", "Linux x64", "WebAssembly" };
        ImGui::Combo("Target Platform", &build_target, targets, IM_ARRAYSIZE(targets));
        ImGui::Spacing();
        ImGui::Checkbox("Optimise (O2)", &optimize);
        ImGui::Checkbox("Strip debug symbols", &strip_debug);
        ImGui::Spacing();
        ImGui::SeparatorText("Output");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("Output Directory", output_dir, sizeof(output_dir));
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.55f, 0.15f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.20f, 0.70f, 0.20f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.10f, 0.40f, 0.10f, 1.f));
        if (ImGui::Button("Build Now", ImVec2(-1, 0)))
            AddLog(LOG_INFO, "Build started for target: %s", targets[build_target]);
        ImGui::PopStyleColor(3);
    }

    ImGui::EndChild(); // content

    // ── Footer buttons ────────────────────────────────────────────────────
    ImGui::Separator();
    float w = (ImGui::GetContentRegionAvail().x - 8.f) / 2.f;
    if (ImGui::Button("Apply & Close", ImVec2(w, 0)))
    {
        AddLog(LOG_INFO, "Preferences saved");
        preferences = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(w, 0)))
        preferences = false;

    ImGui::End();
}