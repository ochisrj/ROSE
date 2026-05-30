#include<filesystem>
namespace fs = std::filesystem;

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include "implot.h"
#include "implot_internal.h"

#include "implot3d.h"
#include "implot3d_internal.h"

#include"ShaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"

// #include "font.h"
#include "menubar.h"
#include"Texture.h"
#include"Camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<stb/stb_image.h>
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


GLfloat vertices[] =
{ //     COORDINATES     /        COLORS      /   TexCoord  //
    -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
    -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
     0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
     0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
     0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	2.5f, 5.0f
};

// Indices for vertices order
GLuint indices[] =
{
    0, 1, 2,
    0, 2, 3,
    0, 1, 4,
    1, 2, 4,
    2, 3, 4,
    3, 0, 4
};




int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int width = 800;
    int height = 600;

    GLFWwindow* window = glfwCreateWindow(width, height, "ROSE game engine", NULL, NULL);
    if (window == NULL) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
    gladLoadGL();

    Shader shaderProgram("default.vert", "default.frag");

    VAO VAO1;
    VAO1.Bind();

    VBO VBO1(vertices, sizeof(vertices));
    EBO EBO1(indices, sizeof(indices));

    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();


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

    /*
        ImFontConfig font_cfg;
        font_cfg.FontDataOwnedByAtlas = false;
        io.Fonts->AddFontFromMemoryTTF(cascadiacode,cascadiacodesize, 17.0f, &font_cfg, io.Fonts->GetGlyphRangesThai());
    */


    stbi_set_flip_vertically_on_load(true);

    std::string parentDir = (fs::current_path().fs::path::parent_path()).string();
    std::string texPath = "/ROSE/";
    std::string fullPath = parentDir + texPath + "20011.jpg";

    std::cout << "Texture Full Path: " << fullPath << std::endl;
    Texture Student((parentDir + texPath + "20011.jpg").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    Student.texUnit(shaderProgram, "tex0", 0);


    glEnable(GL_DEPTH_TEST);

    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

    while (!glfwWindowShouldClose(window))
    {

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.Activate();

        camera.Inputs(window);

        camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");

        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        if (fbWidth != camera.width || fbHeight != camera.height)
        {
            camera.width = fbWidth;
            camera.height = fbHeight;
            glViewport(0, 0, fbWidth, fbHeight);
        }

        Student.Bind();
        VAO1.Bind();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        MenuBar::Draw();

        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    };


    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot3D::DestroyContext();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    Student.Delete();
    shaderProgram.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}