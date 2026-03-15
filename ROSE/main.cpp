#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "implot.h"
#include "implot3d.h"
#include "font.h"

#include <stack>
#include <vector>
#include <sstream>
#include <map> 
#include <math.h>
#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>



// Shader sources
const char* vertexShaderSource = "#version 330 core\n layout (location = 0) in vec3 aPos;\n void main() { gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0); }\0";
const char* fragmentShaderSource = "#version 330 core\n out vec4 FragColor;\n void main() { FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f); }\n\0";

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int width = 1024;
    int height = 768;

    GLFWwindow* window = glfwCreateWindow(width, height, "Mini GeoGebra - C++/OpenGL", NULL, NULL);
    if (window == NULL) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, width, height);

    // --- Shader & Buffer Setup (Same as your code) ---
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLfloat vertices[] = { -0.5f, -0.288f, 0.0f, 0.5f, -0.288f, 0.0f, 0.0f, 0.577f, 0.0f };
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // --- Initialize ImGUI & ImPlot ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImPlot3D::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImFontConfig font_cfg;
    font_cfg.FontDataOwnedByAtlas = false;
    io.Fonts->AddFontFromMemoryTTF(cascadiacode, cascadiacodesize, 18.0f, &font_cfg, io.Fonts->GetGlyphRangesThai());

    // --- State Variables ---
    bool show_plot_2d = false;
    bool config_window = false;
    bool example = true;
    static char formula_buffer[128] = "sin(x)"; // Default formula

    // Main while loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw background triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. TOP MENU BAR
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Files"))
            {
                if (ImGui::MenuItem("Open")) { /* Logic Open */ }
                ImGui::Separator();
                if (ImGui::MenuItem("Close", "Alt+F4")) { glfwSetWindowShouldClose(window, true); }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Tools"))
            {
                ImGui::MenuItem("Plot2D", NULL, &show_plot_2d);
                ImGui::MenuItem("RS config", NULL, &config_window);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (example)
        {
            ImGui::Begin("test",&example);
            ImGui::BulletText("First Item");
            ImGui::BulletText("Second Item");
            ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 200.0f);
            ImGui::Text("This long sentence will automatically wrap once it hits the 200px limit.");
            ImGui::PopTextWrapPos();
            ImGui::Text("Left Side");
            ImGui::SameLine(ImGui::GetWindowWidth() - 100);
            ImGui::Text("Right Side");
            ImGui::Text("Left Side");
            ImGui::SameLine(ImGui::GetWindowWidth() - 100);
            ImGui::Text("Right Side");
            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + 100, pos.y + 20), IM_COL32(255, 255, 0, 100));
            ImGui::Text("Highlighted!");
            ImGui::Text("This is normal text.");
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "This is Orange text.");
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "This is Cyan text.");
            ImGui::TextDisabled("This is 'Disabled' text (usually gray).");

            const char* text = "ADAPTIVE MATHEMATICAL PLOTTER";
            for (int i = 0; i < strlen(text); i++) {
                float hue = i * 0.1f;
                ImGui::TextColored(ImColor::HSV(hue, 0.6f, 1.0f), "%c", text[i]);
                ImGui::SameLine(0.0f, 0.0f); // Keep letters on the same line with no spacing
            }
            ImGui::NewLine();

            // Success Status
            ImGui::TextColored(ImVec4(0, 1, 0, 1), " (V) ");
            ImGui::SameLine();
            ImGui::Text("System Online");
            ImGui::Text("test updare");
            ImGui::Text("test COde");
            ImGui::End();
        }


        // 2. PLOT 2D WINDOW (GeoGebra Style)
        if (show_plot_2d)
        {
            ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);
            if (ImGui::Begin("2D Function Visualizer", &show_plot_2d))
            {
                ImGui::Text("Enter Function:");
                ImGui::SetNextItemWidth(-1);
                ImGui::InputText("##formula", formula_buffer, sizeof(formula_buffer));
                ImGui::TextDisabled("Try: x, x*x, sin(x), cos(x), tan(x)");

                ImGui::Separator();

                if (ImPlot::BeginPlot("##Graph", ImVec2(-1, -1)))
                {
                    ImPlot::SetupAxes("x", "f(x)");
                    ImPlotRect limits = ImPlot::GetPlotLimits();

                    // Generate points dynamically based on view
                    static double x_data[1000], y_data[1000];
                    double step = (limits.X.Max - limits.X.Min) / 999.0;

                    for (int i = 0; i < 1000; ++i) {
                        double x = limits.X.Min + (i * step);
                        double y = 0;

                        // Simple Parser Logic
                        std::string f = formula_buffer;
                        if (f == "x") y = x;
                        else if (f == "x*x" || f == "x^2") y = x * x;
                        else if (f == "sin(x)") y = sin(x);
                        else if (f == "cos(x)") y = cos(x);
                        else if (f == "tan(x)") y = tan(x);
                        else if (f == "sqrt(x)") y = sqrt(x);
                        else y = 0;

                        x_data[i] = x;
                        y_data[i] = y;
                    }

                    ImPlot::SetNextLineStyle(ImVec4(0, 1, 0.7f, 1), 2.0f);
                    ImPlot::PlotLine("f(x)", x_data, y_data, 1000);
                    ImPlot::EndPlot();
                }
            }
            ImGui::End();
        }

        if (config_window)
        {
            ImGui::Begin("Rs config");
            ImGui::TextWrapped("Hello World, This is my personal project that i make it like my hobby. i like to watch behind the sence in many different game");
            char buffer[256] = "";
            ImGui::InputText("Text", buffer, sizeof(buffer));
            ImGui::TextWrapped("Hello, World!");
            float value = 1.23f;
            ImGui::TextWrapped("The value is: %f", value);
            ImGui::TextLinkOpenURL("https://bkwschool.ac.th/");
            ImGui::Text("test update");

            ImGui::End();
        }


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot3D::DestroyContext();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
