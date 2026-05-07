#ifndef SCENE_H
#define SCENE_H

#include "../ECS/Entity.h"
#include "../ECS/Component.h"
#include <string>
#include <vector>

class Scene {
public:
    Scene(const std::string& name);

    EntityID CreateEntity(const std::string& tag = "Entity");
    void DestroyEntity(EntityID id);

    EntityID GetRootEntity() const { return m_RootEntity; }
    const std::string& GetName() const { return m_Name; }

    void OnUpdate(float deltaTime);
    void OnFixedUpdate(float fixedDeltaTime);
    void OnRender();
    void OnEditorGUI();

    bool Save(const std::string& filepath);
    bool Load(const std::string& filepath);

    const std::vector<EntityID>& GetEntities() const { return m_Entities; }

private:
    std::string m_Name;
    EntityID m_RootEntity;
    std::vector<EntityID> m_Entities;
};

#endif
