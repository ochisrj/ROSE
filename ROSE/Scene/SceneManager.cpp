#include <glad/glad.h>
#include "SceneManager.h"
#include "../Core/Log.h"
#include "../Core/Event.h"

SceneManager& SceneManager::Instance() {
    static SceneManager instance;
    return instance;
}

void SceneManager::AddScene(std::unique_ptr<Scene> scene) {
    m_Scenes[scene->GetName()] = std::move(scene);
    if (!m_ActiveScene) {
        m_ActiveScene = m_Scenes.begin()->second.get();
    }
}

void SceneManager::SetActiveScene(const std::string& name) {
    auto it = m_Scenes.find(name);
    if (it != m_Scenes.end()) {
        m_ActiveScene = it->second.get();
        ROSE_LOG_INFO("Switched to scene '%s'", name.c_str());
        Event::Publish<SceneChangeEvent>("scene_changed", {name.c_str()});
    } else {
        ROSE_LOG_ERROR("Scene '%s' not found", name.c_str());
    }
}

Scene* SceneManager::GetScene(const std::string& name) const {
    auto it = m_Scenes.find(name);
    return (it != m_Scenes.end()) ? it->second.get() : nullptr;
}

void SceneManager::RemoveScene(const std::string& name) {
    if (m_ActiveScene && m_ActiveScene->GetName() == name) {
        m_ActiveScene = nullptr;
    }
    m_Scenes.erase(name);
}

void SceneManager::Update(float deltaTime) {
    if (m_ActiveScene) m_ActiveScene->OnUpdate(deltaTime);
}

void SceneManager::FixedUpdate(float fixedDeltaTime) {
    if (m_ActiveScene) m_ActiveScene->OnFixedUpdate(fixedDeltaTime);
}

void SceneManager::Render() {
    if (m_ActiveScene) m_ActiveScene->OnRender();
}

void SceneManager::EditorGUI() {
    if (m_ActiveScene) m_ActiveScene->OnEditorGUI();
}
