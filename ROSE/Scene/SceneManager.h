#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "Scene.h"
#include <memory>
#include <unordered_map>
#include <string>

class SceneManager {
public:
    static SceneManager& Instance();

    void AddScene(std::unique_ptr<Scene> scene);
    void SetActiveScene(const std::string& name);
    Scene* GetActiveScene() const { return m_ActiveScene; }
    Scene* GetScene(const std::string& name) const;
    void RemoveScene(const std::string& name);

    void Update(float deltaTime);
    void FixedUpdate(float fixedDeltaTime);
    void Render();
    void EditorGUI();

private:
    SceneManager() = default;
    std::unordered_map<std::string, std::unique_ptr<Scene>> m_Scenes;
    Scene* m_ActiveScene = nullptr;
};

#endif
