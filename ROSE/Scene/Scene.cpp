#include <glad/glad.h>
#include "Scene.h"
#include "../Core/Log.h"
#include <algorithm>

Scene::Scene(const std::string& name) : m_Name(name), m_RootEntity(INVALID_ENTITY) {
    m_RootEntity = EntityRegistry::Instance().CreateEntity();
    EntityRegistry::Instance().AddComponent<TagComponent>(m_RootEntity, "SceneRoot");
    ROSE_LOG_INFO("Scene '%s' created", name.c_str());
}

EntityID Scene::CreateEntity(const std::string& tag) {
    EntityID id = EntityRegistry::Instance().CreateEntity();
    EntityRegistry::Instance().AddComponent<TransformComponent>(id);
    EntityRegistry::Instance().AddComponent<TagComponent>(id, tag);
    m_Entities.push_back(id);
    return id;
}

void Scene::DestroyEntity(EntityID id) {
    EntityRegistry::Instance().DestroyEntity(id);
    m_Entities.erase(std::remove(m_Entities.begin(), m_Entities.end(), id), m_Entities.end());
}

void Scene::OnUpdate(float deltaTime) {
    for (auto& id : m_Entities) {
        if (!EntityRegistry::Instance().IsAlive(id)) continue;
    }
}

void Scene::OnFixedUpdate(float fixedDeltaTime) {
    for (auto& id : m_Entities) {
        if (!EntityRegistry::Instance().IsAlive(id)) continue;
    }
}

void Scene::OnRender() {
}

void Scene::OnEditorGUI() {
}

bool Scene::Save(const std::string& filepath) {
    ROSE_LOG_INFO("Saving scene '%s' to '%s'", m_Name.c_str(), filepath.c_str());
    return true;
}

bool Scene::Load(const std::string& filepath) {
    ROSE_LOG_INFO("Loading scene from '%s'", filepath.c_str());
    return true;
}
