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
                    ImVec2 window_pos = ImVec2(100, 90);
                    ImGui::SetWindowPos(window_pos, ImGuiCond_Always);
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
            if (ImGui::MenuItem("Image Viewer"));
            ImGui::EndMenu();
        }

        static int ea = 0;
        if (ImGui::RadioButton("window", &ea, 0)) {}
        ImGui::SameLine();
        if (ImGui::RadioButton("full screen", &ea, 1)) {}

        float fps = ImGui::GetIO().Framerate;
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 100);
        ImGui::Text("FPS: %.1f", fps);

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
    if (fps_window)
    {
        ImGui::SetNextWindowBgAlpha(0.35f);
        ImGui::Begin("FPS window", &fps_window, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::PopStyleColor();
        ImGui::Text("CPU: ");
        ImGui::Text("GPU: ");
        

        
        ImGui::End();
    }

}

void debugtool::DrawTextFormat()
{

    if (text_formatting)
    {
        ImGui::Begin("Text Formmatting", &text_formatting);
        ImGui::SeparatorText("Text");
        ImGui::Text("This is test message for my c++ UI pratice");

        ImGui::SeparatorText("Text Wrap");
        ImGui::TextWrapped("Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic");

        ImGui::SeparatorText("Text Bullet");
        ImGui::BulletText("text bullet vro");
        ImGui::BulletText("text bullet vro");
        ImGui::BulletText("text bullet vro");

        ImGui::SeparatorText("Button Style");
        static bool checked = false;
        ImGui::Checkbox("box", &checked);
        if (checked)
        {
            std::cout << "I know it" << std::endl;
        }
        if (ImGui::Button("Window"))
        {
        }


        ImGui::End();
    }


}

void debugtool::DrawImageViewer()
{

}