#ifndef SYSTEM_H
#define SYSTEM_H

#include "Entity.h"
#include <memory>
#include <vector>

class System {
public:
    virtual ~System() = default;
    virtual void Update(float deltaTime) = 0;
    virtual void FixedUpdate(float fixedDeltaTime) {}
    virtual void Render() {}
    virtual void EditorGUI() {}
};

class SystemRegistry {
public:
    static SystemRegistry& Instance();

    void AddSystem(std::unique_ptr<System> system);
    void UpdateAll(float deltaTime);
    void FixedUpdateAll(float fixedDeltaTime);
    void RenderAll();
    void EditorGUIAll();

private:
    SystemRegistry() = default;
    std::vector<std::unique_ptr<System>> m_Systems;
};

#endif
