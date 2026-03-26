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
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform float size;\n" // เพิ่มตัวรับขนาด
"void main()\n"
"{\n"
"   gl_Position = vec4(size * aPos.x, size * aPos.y, size * aPos.z, 1.0);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 color;\n" // เพิ่มตัวรับสี
"void main()\n"
"{\n"
"   FragColor = color;\n"
"}\n\0";

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int width = 1024;
    int height = 768;

    GLFWwindow* window = glfwCreateWindow(width, height, "ROSE game engine", NULL, NULL);
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

    GLfloat vertices[] = { 
        -0.5f, -0.288f, 0.0f, 
         0.5f, -0.288f, 0.0f, 
         0.0f,  0.577f, 0.0f 
    };

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
    
    int ea = 0;


    bool trideb = false;
    bool text_formatting = false;
    bool demo_window = false;


    bool fps_window = false;


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

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {

                if(ImGui::MenuItem("Save")){}
                if(ImGui::MenuItem("Save as")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Open File")){}
                if (ImGui::MenuItem("Open Folder")){}
                
                ImGui::Separator();
                if (ImGui::BeginMenu("Preference"))
                {
                    static float sizew = 1.0f;
                    ImGui::Checkbox("SHOW FPS", &fps_window);
                    ImGui::SliderFloat("float", &sizew,0.5f, 1.0f);
                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::BeginMenu("Window"))
                {
                    if (ImGui::MenuItem("Demo","", &demo_window)){}
                    if (ImGui::MenuItem("Text Demo","",&text_formatting)){}
                    if (ImGui::MenuItem("Triangle Debug","",&trideb)){}

                    ImGui::EndMenu();
                }
                    
                ImGui::EndMenu();
            }

            if (ImGui::RadioButton("window",&ea,0)) {}
            ImGui::SameLine();
            if (ImGui::RadioButton("full screen", &ea, 1)) {}

            ImGui::EndMainMenuBar();

        }
        


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


        if (trideb)
        {   
            static bool drawTriangle = false;
            static float size = 1.0f;
            static float color[4] = { 0.8f, 0.3f, 0.02f, 1.0f };
            glUseProgram(shaderProgram);
            glUniform1f(glGetUniformLocation(shaderProgram, "size"), size);
            glUniform4f(glGetUniformLocation(shaderProgram, "color"), color[0], color[1], color[2], color[3]);
            glBindVertexArray(VAO);


            // ImGUI window creation
            ImGui::Begin("Triangle Debug",&trideb);
            // Text that appears in the window
            ImGui::Text("Hello there adventurer!");
            // Checkbox that appears in the window
            ImGui::Checkbox("Draw Triangle", &drawTriangle);
            if (drawTriangle)
                // Draw the triangle using the GL_TRIANGLES primitive
                glDrawArrays(GL_TRIANGLES, 0, 3);
            // Slider that appears in the window
            ImGui::SliderFloat("Size", &size, 0.5f, 2.0f);
            // Fancy color editor that appears in the window
            ImGui::ColorEdit4("Color", color);
            // Ends the window
            ImGui::End();
        }

        if (fps_window)
        {
            ImGui::Begin("FPS window", &fps_window);
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::End();
        }
        

        if (demo_window)
        {
            ImGui::ShowDemoWindow(&demo_window);
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
