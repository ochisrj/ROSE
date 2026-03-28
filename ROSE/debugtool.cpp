#include "debugtool.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include "implot.h"
#include "implot_internal.h"

#include "implot3d.h"
#include "implot3d_internal.h"

/*
#include "stb_image.h"
#include "main.cpp"
*/

#include <iostream>
#include <cmath>


debugtool::debugtool()
{

}

void debugtool::DrawUI(ImGuiIO& io)
{
	DrawDemo();
    DrawDemoPlot();
    DrawDemoPlot3D();
	DrawFPS();
	DrawTextFormat();
	DrawMenuBar();
    DrawImageViewer();
    DrawCalculator();
}

void debugtool::DrawMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {

            if (ImGui::MenuItem("Save")) {}
            if (ImGui::MenuItem("Save as")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Open File")) {}
            if (ImGui::MenuItem("Open Folder")) {}
            ImGui::Separator();
            if (ImGui::BeginMenu("Preference"))
            {
                if (ImGui::Checkbox("SHOW FPS",&fps_window))
                {

                }
                static float sizew = 1.0f;
                ImGui::SliderFloat("float", &sizew, 0.5f, 1.0f);
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Quit"))
            {

            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::BeginMenu("Window"))
            {
                if (ImGui::MenuItem("Demo", "", &demo_window)) {}
                if (ImGui::MenuItem("Demo Plot", "" , &demo_plot)) {}
                if (ImGui::MenuItem("Demo Plot3D", "" , &demo_plot3D)) {}
                if (ImGui::MenuItem("Text Demo", "", &text_formatting)) {}

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tools"))
        {
            if (ImGui::MenuItem("Image Viewer",NULL,&image_viewer)){}
            if (ImGui::MenuItem("Calculator",NULL,&calculator)) {}
            ImGui::EndMenu();
        }

        static int ea = 0;
        if (ImGui::RadioButton("window", &ea, 0)) {}
        ImGui::SameLine();
        if (ImGui::RadioButton("full screen", &ea, 1)) {}

        float fps = ImGui::GetIO().Framerate;
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 250);
        ImGui::Text("FPS: %.1f | some text", fps);

        ImGui::EndMainMenuBar();

    }
}

void debugtool::DrawDemo()
{
    if (demo_window)
    {
        ImGui::ShowDemoWindow(&demo_window);
    }

}

void debugtool::DrawDemoPlot()
{
    if (demo_plot)
    {
        ImPlot::ShowDemoWindow(&demo_plot);
    }
}

void debugtool::DrawDemoPlot3D()
{
    if (demo_plot3D)
    {
        ImPlot3D::ShowDemoWindow(&demo_plot3D);
    }
}


void debugtool::DrawFPS()
{
    if (!fps_window) return;

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

    ImGui::Begin("FPS window", &fps_window, flags);

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


void debugtool::DrawTextFormat()
{
    if (!text_formatting) return;

    ImGui::SetNextWindowSize(ImVec2(620, 720), ImGuiCond_FirstUseEver);
    ImGui::Begin("ImGui Feature Showcase", &text_formatting);

    // ── 1. TEXT STYLES ─────────────────────────────────────────────────────
    if (ImGui::CollapsingHeader("Text Styles", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Normal text");
        ImGui::TextDisabled("Disabled / dimmed text");
        ImGui::TextWrapped("Wrapped: Lorem ipsum dolor sit amet, consectetur "
            "adipiscing elit. Sed do eiusmod tempor incididunt ut labore.");
        ImGui::LabelText("Label", "Value next to label");
        ImGui::BulletText("Bullet item A");
        ImGui::BulletText("Bullet item B");
        ImGui::BulletText("Bullet item C");
        ImGui::SeparatorText("Coloured Text");
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Red");   ImGui::SameLine();
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Green"); ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.3f, 0.7f, 1, 1), "Blue"); ImGui::SameLine();
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Yellow");
    }

    // ── 2. BUTTONS ─────────────────────────────────────────────────────────
    if (ImGui::CollapsingHeader("Buttons"))
    {
        static int btn_clicks = 0;
        if (ImGui::Button("Normal Button"))    btn_clicks++;
        ImGui::SameLine();
        if (ImGui::SmallButton("Small"))       btn_clicks++;
        ImGui::SameLine();
        if (ImGui::ArrowButton("##left", ImGuiDir_Left))  btn_clicks++;
        ImGui::SameLine();
        if (ImGui::ArrowButton("##right", ImGuiDir_Right)) btn_clicks++;
        ImGui::SameLine();
        ImGui::Text("Clicks: %d", btn_clicks);

        // Coloured button via PushStyleColor
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.1f, 0.1f, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.2f, 0.2f, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.3f, 0.3f, 1));
        if (ImGui::Button("Red Danger Button")) {}
        ImGui::PopStyleColor(3);

        // Wide button
        ImGui::Button("Wide Button", ImVec2(-1, 0));

        static bool toggle = false;
        if (toggle) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1));
        if (ImGui::Button(toggle ? "ON " : "OFF")) toggle = !toggle;
        if (toggle) ImGui::PopStyleColor();
        ImGui::SameLine(); ImGui::TextDisabled("Toggle button");
    }

    // ── 3. CHECKBOXES, RADIO, COMBO ────────────────────────────────────────
    if (ImGui::CollapsingHeader("Toggles & Selection"))
    {
        static bool cb[3] = { true, false, true };
        ImGui::Checkbox("Option A", &cb[0]); ImGui::SameLine();
        ImGui::Checkbox("Option B", &cb[1]); ImGui::SameLine();
        ImGui::Checkbox("Option C", &cb[2]);

        static int radio = 0;
        ImGui::RadioButton("Radio 0", &radio, 0); ImGui::SameLine();
        ImGui::RadioButton("Radio 1", &radio, 1); ImGui::SameLine();
        ImGui::RadioButton("Radio 2", &radio, 2);
        ImGui::Text("Selected radio: %d", radio);

        static int combo_idx = 0;
        const char* combo_items[] = { "Sprite", "Mesh", "Light", "Camera", "Audio" };
        ImGui::Combo("Entity Type", &combo_idx, combo_items, IM_ARRAYSIZE(combo_items));

        static int listbox_idx = 0;
        ImGui::ListBox("List", &listbox_idx, combo_items, IM_ARRAYSIZE(combo_items), 3);
    }

    // ── 4. SLIDERS & DRAG ──────────────────────────────────────────────────
    if (ImGui::CollapsingHeader("Sliders & Drag"))
    {
        static float f1 = 0.5f, f2 = 128.f;
        static int   i1 = 42;
        static float v2[2] = { 0.2f, 0.8f };
        static float v3[3] = { 0.1f, 0.5f, 0.9f };

        ImGui::SliderFloat("Float", &f1, 0.f, 1.f);
        ImGui::SliderFloat("Logarithmic", &f2, 1.f, 1024.f, "%.0f", ImGuiSliderFlags_Logarithmic);
        ImGui::SliderInt("Integer", &i1, 0, 100);
        ImGui::SliderFloat2("Vec2", v2, 0.f, 1.f);
        ImGui::SliderFloat3("Vec3", v3, 0.f, 1.f);

        static float drag_f = 0.f;
        static int   drag_i = 0;
        ImGui::DragFloat("Drag Float", &drag_f, 0.01f, -10.f, 10.f, "%.3f");
        ImGui::DragInt("Drag Int", &drag_i, 1, -100, 100);
    }

    // ── 5. INPUT FIELDS ────────────────────────────────────────────────────
    if (ImGui::CollapsingHeader("Input Fields"))
    {
        static char text_buf[128] = "Edit me!";
        static char multiline[512] = "Line 1\nLine 2\nLine 3";
        static float inp_f = 0.f;
        static int   inp_i = 0;

        ImGui::InputText("Text Input", text_buf, sizeof(text_buf));
        ImGui::InputTextMultiline("Multiline", multiline, sizeof(multiline), ImVec2(-1, 80));
        ImGui::InputFloat("Input Float", &inp_f, 0.1f, 1.f, "%.3f");
        ImGui::InputInt("Input Int", &inp_i);
    }

    // ── 6. COLORS ──────────────────────────────────────────────────────────
    if (ImGui::CollapsingHeader("Color Pickers"))
    {
        static ImVec4 color3 = { 0.4f, 0.7f, 0.3f, 1.f };
        static ImVec4 color4 = { 0.8f, 0.2f, 0.5f, 0.8f };
        ImGui::ColorEdit3("RGB Color", (float*)&color3);
        ImGui::ColorEdit4("RGBA Color", (float*)&color4);
        ImGui::ColorPicker4("Color Picker", (float*)&color4,
            ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_AlphaBar);
    }

    // ── 7. LAYOUT HELPERS ──────────────────────────────────────────────────
    if (ImGui::CollapsingHeader("Layout"))
    {
        // Columns (legacy)
        ImGui::SeparatorText("Columns (3)");
        ImGui::Columns(3, "cols");
        ImGui::Separator();
        ImGui::Text("Name");   ImGui::NextColumn();
        ImGui::Text("Type");   ImGui::NextColumn();
        ImGui::Text("Value");  ImGui::NextColumn();
        ImGui::Separator();
        const char* rows[][3] = {
            {"position", "Vec3",  "0,0,0"},
            {"scale",    "Vec3",  "1,1,1"},
            {"visible",  "bool",  "true" },
        };
        for (auto& r : rows) {
            ImGui::Text("%s", r[0]); ImGui::NextColumn();
            ImGui::Text("%s", r[1]); ImGui::NextColumn();
            ImGui::Text("%s", r[2]); ImGui::NextColumn();
        }
        ImGui::Columns(1);

        // Tables (new API)
        ImGui::SeparatorText("Table");
        if (ImGui::BeginTable("tbl", 4,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("ID");
            ImGui::TableSetupColumn("Component");
            ImGui::TableSetupColumn("Active");
            ImGui::TableSetupColumn("Priority");
            ImGui::TableHeadersRow();
            for (int r = 0; r < 5; r++) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn(); ImGui::Text("%d", r + 1);
                ImGui::TableNextColumn(); ImGui::Text("System %d", r);
                ImGui::TableNextColumn(); ImGui::Text("%s", r % 2 ? "yes" : "no");
                ImGui::TableNextColumn(); ImGui::Text("%d", r * 3);
            }
            ImGui::EndTable();
        }

        // Child windows
        ImGui::SeparatorText("Child Window (scrollable)");
        ImGui::BeginChild("child_scroll", ImVec2(-1, 80), true);
        for (int i = 0; i < 20; i++)
            ImGui::Text("Scrollable item %d", i);
        ImGui::EndChild();

        // Groups & SameLine
        ImGui::SeparatorText("Group & SameLine");
        ImGui::BeginGroup();
        ImGui::Text("Group A");
        ImGui::Button("Btn 1"); ImGui::SameLine();
        ImGui::Button("Btn 2");
        ImGui::EndGroup();
        ImGui::SameLine(0, 30);
        ImGui::BeginGroup();
        ImGui::Text("Group B");
        ImGui::Button("Btn 3"); ImGui::SameLine();
        ImGui::Button("Btn 4");
        ImGui::EndGroup();
    }

    // ── 8. TABS ────────────────────────────────────────────────────────────
    if (ImGui::CollapsingHeader("Tabs"))
    {
        if (ImGui::BeginTabBar("MyTabs"))
        {
            if (ImGui::BeginTabItem("Rendering"))
            {
                static bool vsync = true;
                static int msaa = 4;
                ImGui::Checkbox("VSync", &vsync);
                ImGui::SliderInt("MSAA", &msaa, 1, 16);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Physics"))
            {
                static float gravity = -9.81f;
                ImGui::DragFloat("Gravity", &gravity, 0.01f, -20.f, 0.f);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Audio"))
            {
                static float master = 1.f;
                ImGui::SliderFloat("Master Volume", &master, 0.f, 1.f);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }

    // ── 9. TREE / HIERARCHY ────────────────────────────────────────────────
    if (ImGui::CollapsingHeader("Tree / Hierarchy"))
    {
        if (ImGui::TreeNode("Scene Root"))
        {
            if (ImGui::TreeNode("Player"))
            {
                ImGui::BulletText("Transform");
                ImGui::BulletText("Rigidbody");
                if (ImGui::TreeNode("Mesh")) {
                    ImGui::BulletText("Vertices: 1024");
                    ImGui::BulletText("Material: player_skin");
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Environment")) {
                ImGui::BulletText("Sky");
                ImGui::BulletText("Terrain");
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
    }

    // ── 10. TOOLTIP & POPUP ────────────────────────────────────────────────
    if (ImGui::CollapsingHeader("Tooltip & Popup"))
    {
        ImGui::Text("Hover me");
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("I'm a tooltip!\nVery helpful.");

        static bool popup_open = false;
        if (ImGui::Button("Open Popup")) popup_open = true;
        if (popup_open)
        {
            ImGui::OpenPopup("MyPopup");
            popup_open = false;
        }
        if (ImGui::BeginPopupModal("MyPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("This is a modal popup.");
            ImGui::Separator();
            if (ImGui::Button("OK", ImVec2(120, 0))) ImGui::CloseCurrentPopup();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
    }

    // ── 11. PLOT (quick inline chart) ──────────────────────────────────────
    if (ImGui::CollapsingHeader("Plot Preview"))
    {
        static float sine[128];
        static bool  init = false;
        if (!init) {
            for (int i = 0; i < 128; i++)
                sine[i] = sinf((float)i * 0.1f);
            init = true;
        }
        ImGui::PlotLines("Sine Wave", sine, 128, 0, NULL, -1.5f, 1.5f, ImVec2(-1, 60));

        static float hist[] = { 0.1f,0.4f,0.9f,0.6f,0.3f,0.7f,0.5f,0.8f };
        ImGui::PlotHistogram("Histogram", hist, IM_ARRAYSIZE(hist), 0, NULL, 0.f, 1.f, ImVec2(-1, 60));
    }

    ImGui::End();
}


void debugtool::DrawImageViewer()
{
    if (image_viewer)
    {
        /*
        int my_image_width = 0;
        int my_image_height = 0;
        GLuint my_image_texture = 0;
        bool ret = LoadTextureFromFile("C:/Users/Ochi/Downloads/ROSE/ROSE/20011.jpg", &my_image_texture, &my_image_width, &my_image_height);
        IM_ASSERT(ret);
        */
        ImGui::Begin("OpenGL Texture Text", &image_viewer);
        ImGui::Text("te");
        /*
        ImGui::Text("pointer = %p", my_image_texture);
        ImGui::Text("size = %d x %d", my_image_width, my_image_height);
        ImGui::Image((void*)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
        */
        ImGui::End();
    }
}

void debugtool::DrawCalculator()
{
    if (!calculator) return;

    // ── persistent state ───────────────────────────────────────────────────
    static double displayVal = 0.0;
    static double storedVal = 0.0;
    static char   op = 0;
    static bool   newNumber = true;
    static bool   justEvaled = false;
    static char   displayBuf[64] = "0";
    static char   exprBuf[64] = "";   // small expression hint above number

    // ── helpers ────────────────────────────────────────────────────────────
    auto refreshBuf = [&]() {
        if (displayVal == (long long)displayVal &&
            displayVal < 1e15 && displayVal > -1e15)
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

    auto opPressed = [&](char o) {
        // chain: if already have op, evaluate first
        if (op && !newNumber) {
            double rhs = displayVal;
            switch (op) {
            case '+': displayVal = storedVal + rhs; break;
            case '-': displayVal = storedVal - rhs; break;
            case '*': displayVal = storedVal * rhs; break;
            case '/': displayVal = (rhs != 0) ? storedVal / rhs : 0.0; break;
            case 'p': displayVal = pow(storedVal, rhs); break;
            case '%': displayVal = storedVal * (rhs / 100.0); break;
            }
            refreshBuf();
        }
        storedVal = displayVal;
        op = o;
        newNumber = true;
        justEvaled = false;
        // build expression hint
        char opCh = (o == 'p') ? '^' : o;
        snprintf(exprBuf, sizeof(exprBuf), "%s %c", displayBuf, opCh);
        };

    auto evalPressed = [&]() {
        if (op == 0) return;
        double rhs = displayVal;
        switch (op) {
        case '+': displayVal = storedVal + rhs; break;
        case '-': displayVal = storedVal - rhs; break;
        case '*': displayVal = storedVal * rhs; break;
        case '/': displayVal = (rhs != 0) ? storedVal / rhs : 0.0; break;
        case 'p': displayVal = pow(storedVal, rhs); break;
        case '%': displayVal = storedVal * (rhs / 100.0); break;
        }
        op = 0; justEvaled = true; newNumber = true;
        refreshBuf();
        snprintf(exprBuf, sizeof(exprBuf), "");
        };

    auto clearPressed = [&]() {
        displayVal = storedVal = 0;
        op = 0; newNumber = true; justEvaled = false;
        snprintf(displayBuf, sizeof(displayBuf), "0");
        snprintf(exprBuf, sizeof(exprBuf), "");
        };

    auto dotPressed = [&]() {
        if (newNumber) {
            snprintf(displayBuf, sizeof(displayBuf), "0.");
            displayVal = 0; newNumber = false;
        }
        else if (!strchr(displayBuf, '.')) {
            size_t len = strlen(displayBuf);
            if (len + 2 < sizeof(displayBuf)) {
                displayBuf[len] = '.'; displayBuf[len + 1] = '\0';
            }
        }
        };

    auto backPressed = [&]() {
        if (justEvaled) return;
        size_t len = strlen(displayBuf);
        if (len > 1) { displayBuf[len - 1] = '\0'; displayVal = atof(displayBuf); }
        else { snprintf(displayBuf, sizeof(displayBuf), "0"); displayVal = 0; }
        };

    // ── keyboard shortcuts ─────────────────────────────────────────────────
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantTextInput)
    {
        for (int d = 0; d <= 9; d++) {
            if (ImGui::IsKeyPressed((ImGuiKey)((int)ImGuiKey_Keypad0 + d), false) ||
                ImGui::IsKeyPressed((ImGuiKey)((int)ImGuiKey_0 + d), false))
                digitPressed(d);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_KeypadAdd, false)) opPressed('+');
        if (ImGui::IsKeyPressed(ImGuiKey_KeypadSubtract, false)) opPressed('-');
        if (ImGui::IsKeyPressed(ImGuiKey_KeypadMultiply, false)) opPressed('*');
        if (ImGui::IsKeyPressed(ImGuiKey_KeypadDivide, false)) opPressed('/');
        if (ImGui::IsKeyPressed(ImGuiKey_KeypadEnter, false) ||
            ImGui::IsKeyPressed(ImGuiKey_Enter, false)) evalPressed();
        if (ImGui::IsKeyPressed(ImGuiKey_Backspace, false)) backPressed();
        if (ImGui::IsKeyPressed(ImGuiKey_Escape, false)) clearPressed();
        if (ImGui::IsKeyPressed(ImGuiKey_KeypadDecimal, false) ||
            ImGui::IsKeyPressed(ImGuiKey_Period, false)) dotPressed();
    }

    // ── window ─────────────────────────────────────────────────────────────
    // Fixed size so the grid never wraps or shifts
    ImGui::SetNextWindowSize(ImVec2(296, 460), ImGuiCond_Always);
    ImGui::Begin("Calculator", &calculator,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

    // ── display panel ──────────────────────────────────────────────────────
    // Available width minus window padding on both sides
    float panelW = ImGui::GetContentRegionAvail().x;

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.08f, 0.10f, 1.f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 4.f);
    ImGui::BeginChild("##disp", ImVec2(panelW, 64), false);

    // Expression hint (small, top-right)
    if (exprBuf[0]) {
        float hw = ImGui::CalcTextSize(exprBuf).x;
        ImGui::SetCursorPos(ImVec2(panelW - hw - 6, 4));
        ImGui::TextDisabled("%s", exprBuf);
    }

    // Main number (large, bottom-right)
    float nw = ImGui::CalcTextSize(displayBuf).x;
    ImGui::SetCursorPos(ImVec2(panelW - nw - 6, 36));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
    ImGui::TextUnformatted(displayBuf);
    ImGui::PopStyleColor();

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    ImGui::Spacing();

    // ── uniform button grid ─────────────────────────────────────────────────
    // 4 columns, equal width, 4 px spacing between buttons
    const float spacing = 4.f;
    const float btnW = (panelW - spacing * 3.f) / 4.f;
    const float btnH = 52.f;
    const ImVec2 B(btnW, btnH);

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing, spacing));

    // ── Color palette ──────────────────────────────────────────────────────
    // Digit buttons  — medium grey
    const ImVec4 C_DIGIT_N(0.22f, 0.22f, 0.26f, 1.f);
    const ImVec4 C_DIGIT_H(0.30f, 0.30f, 0.36f, 1.f);
    const ImVec4 C_DIGIT_A(0.15f, 0.15f, 0.18f, 1.f);
    // Operator buttons — amber / orange
    const ImVec4 C_OP_N(0.75f, 0.48f, 0.06f, 1.f);
    const ImVec4 C_OP_H(0.90f, 0.60f, 0.12f, 1.f);
    const ImVec4 C_OP_A(0.58f, 0.36f, 0.02f, 1.f);
    // Utility buttons (C, CE, +/-, ⌫) — slate blue
    const ImVec4 C_UTIL_N(0.28f, 0.35f, 0.50f, 1.f);
    const ImVec4 C_UTIL_H(0.36f, 0.45f, 0.65f, 1.f);
    const ImVec4 C_UTIL_A(0.20f, 0.26f, 0.38f, 1.f);
    // Equals button  — vivid teal
    const ImVec4 C_EQ_N(0.08f, 0.58f, 0.53f, 1.f);
    const ImVec4 C_EQ_H(0.12f, 0.72f, 0.66f, 1.f);
    const ImVec4 C_EQ_A(0.05f, 0.42f, 0.38f, 1.f);

    // helpers: push 3 colours, call button, pop
    auto Btn = [&](const char* label, ImVec2 sz,
        ImVec4 cn, ImVec4 ch, ImVec4 ca) -> bool {
            ImGui::PushStyleColor(ImGuiCol_Button, cn);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ch);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ca);
            bool clicked = ImGui::Button(label, sz);
            ImGui::PopStyleColor(3);
            return clicked;
        };
    auto BtnD = [&](const char* lbl) -> bool { return Btn(lbl, B, C_DIGIT_N, C_DIGIT_H, C_DIGIT_A); };
    auto BtnO = [&](const char* lbl) -> bool { return Btn(lbl, B, C_OP_N, C_OP_H, C_OP_A);    };
    auto BtnU = [&](const char* lbl) -> bool { return Btn(lbl, B, C_UTIL_N, C_UTIL_H, C_UTIL_A);  };
    auto BtnE = [&](const char* lbl) -> bool { return Btn(lbl, B, C_EQ_N, C_EQ_H, C_EQ_A);    };

    // ── Row 0: C  CE  +/-  ⌫ ───────────────────────────────────────────────
    if (BtnU("C"))   clearPressed();                                    ImGui::SameLine();
    if (BtnU("CE")) { displayVal = 0; newNumber = true; refreshBuf(); } ImGui::SameLine();
    if (BtnU("+/-")) { displayVal = -displayVal; refreshBuf(); }        ImGui::SameLine();
    if (BtnU("<X"))  backPressed();

    // ── Row 1: sqrt  x^y  %  / ─────────────────────────────────────────────
    if (BtnO("sqrt")) { displayVal = sqrt(fabs(displayVal)); refreshBuf(); newNumber = true; } ImGui::SameLine();
    if (BtnO("x^y"))  opPressed('p');  ImGui::SameLine();
    if (BtnO("%"))    opPressed('%');   ImGui::SameLine();
    if (BtnO("/"))    opPressed('/');

    // ── Row 2: 7  8  9  * ──────────────────────────────────────────────────
    if (BtnD("7")) digitPressed(7); ImGui::SameLine();
    if (BtnD("8")) digitPressed(8); ImGui::SameLine();
    if (BtnD("9")) digitPressed(9); ImGui::SameLine();
    if (BtnO("*")) opPressed('*');

    // ── Row 3: 4  5  6  - ──────────────────────────────────────────────────
    if (BtnD("4")) digitPressed(4); ImGui::SameLine();
    if (BtnD("5")) digitPressed(5); ImGui::SameLine();
    if (BtnD("6")) digitPressed(6); ImGui::SameLine();
    if (BtnO("-")) opPressed('-');

    // ── Row 4: 1  2  3  + ──────────────────────────────────────────────────
    if (BtnD("1")) digitPressed(1); ImGui::SameLine();
    if (BtnD("2")) digitPressed(2); ImGui::SameLine();
    if (BtnD("3")) digitPressed(3); ImGui::SameLine();
    if (BtnO("+")) opPressed('+');

    // ── Row 5: 0  .  (gap)  = ──────────────────────────────────────────────
    // 0 and . stay digit-sized; = is teal.  We insert an invisible dummy for the 3rd cell.
    if (BtnD("0")) digitPressed(0); ImGui::SameLine();
    if (BtnD(".")) dotPressed();    ImGui::SameLine();
    // empty spacer cell — same size, transparent
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
    ImGui::Button("##empty", B);
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    if (BtnE("=")) evalPressed();

    ImGui::PopStyleVar();  // FrameRounding, ItemSpacing

    ImGui::Spacing();
    ImGui::TextDisabled("Numpad / keyboard shortcuts active");

    ImGui::End();
}
