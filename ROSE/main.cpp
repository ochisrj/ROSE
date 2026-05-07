#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <filesystem>
namespace fs = std::filesystem;

#include "Core/Log.h"
#include "Core/Engine.h"
#include "Core/Time.h"
#include "Core/Event.h"
#include "Scene/SceneManager.h"
#include "Resource/ResourceManager.h"
#include "ECS/Entity.h"
#include "ECS/Component.h"
#include "ECS/System.h"

#include "ShaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"
#include "Camera.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include "implot.h"
#include "implot_internal.h"

#include "implot3d.h"
#include "implot3d_internal.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <math.h>

GLfloat vertices[] =
{
    -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 0.0f, 0.0f,
    -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f, 5.0f, 0.0f,
     0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f, 0.0f, 0.0f,
     0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 5.0f, 0.0f,
     0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f, 2.5f, 5.0f
};

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
    auto& engine = Engine::Instance();

    if (!engine.Init(1920, 1080, "ROSE Engine")) {
        return -1;
    }

    stbi_set_flip_vertically_on_load(true);

    std::string parentDir = (fs::current_path().fs::path::parent_path()).string();
    std::string texPath = "/ROSE/";
    std::string fullPath = parentDir + texPath + "20011.jpg";
    std::cout << "Texture Full Path: " << fullPath << std::endl;

    auto scene = std::make_unique<Scene>("DefaultScene");
    EntityRegistry& registry = EntityRegistry::Instance();

    EntityID testEntity = scene->CreateEntity("TestPyramid");
    auto& transform = registry.GetComponent<TransformComponent>(testEntity);
    transform.Position = glm::vec3(0.0f, 0.0f, -2.0f);
    transform.Scale = glm::vec3(1.0f, 1.0f, 1.0f);

    auto& sprite = registry.AddComponent<SpriteComponent>(testEntity);
    sprite.TexturePath = fullPath;
    sprite.Size = glm::vec2(1.0f, 1.0f);
    sprite.Layer = 0;

    ResourceManager::Instance().LoadTexture("test_texture", fullPath.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    ResourceManager::Instance().LoadShader("default_shader", "default.vert", "default.frag");

    SceneManager::Instance().AddScene(std::move(scene));
    SceneManager::Instance().SetActiveScene("DefaultScene");

    engine.Run();

    engine.Shutdown();
    return 0;
}
