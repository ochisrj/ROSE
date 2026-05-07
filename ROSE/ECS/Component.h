#ifndef COMPONENT_H
#define COMPONENT_H

#define GLM_ENABLE_EXPERIMENTAL
#include "Entity.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

struct TransformComponent : ComponentBase {
    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 Rotation = glm::vec3(0.0f);
    glm::vec3 Scale = glm::vec3(1.0f);

    glm::mat4 GetModelMatrix() const {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, Position);
        model = glm::rotate(model, glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, Scale);
        return model;
    }
};

struct SpriteComponent : ComponentBase {
    std::string TexturePath;
    glm::vec2 Size = glm::vec2(1.0f, 1.0f);
    glm::vec4 Color = glm::vec4(1.0f);
    int Layer = 0;
    bool Visible = true;
};

struct CameraComponent : ComponentBase {
    float FOV = 45.0f;
    float NearPlane = 0.1f;
    float FarPlane = 100.0f;
    bool IsActive = false;
};

struct PhysicsComponent : ComponentBase {
    glm::vec2 Velocity = glm::vec2(0.0f);
    glm::vec2 Size = glm::vec2(1.0f);
    bool IsTrigger = false;
    bool IsActive = true;
};

struct ScriptComponent : ComponentBase {
    std::string ScriptName;
};

struct TagComponent : ComponentBase {
    std::string Tag;
    TagComponent() = default;
    TagComponent(const std::string& tag) : Tag(tag) {}
    TagComponent(const char* tag) : Tag(tag) {}
};

struct TilemapComponent : ComponentBase {
    std::string TilesetPath;
    int TileWidth = 16;
    int TileHeight = 16;
    int MapWidth = 0;
    int MapHeight = 0;
    std::vector<int> Tiles;
};

#endif
