#include "System.h"

SystemRegistry& SystemRegistry::Instance() {
    static SystemRegistry instance;
    return instance;
}

void SystemRegistry::AddSystem(std::unique_ptr<System> system) {
    m_Systems.push_back(std::move(system));
}

void SystemRegistry::UpdateAll(float deltaTime) {
    for (auto& system : m_Systems) {
        system->Update(deltaTime);
    }
}

void SystemRegistry::FixedUpdateAll(float fixedDeltaTime) {
    for (auto& system : m_Systems) {
        system->FixedUpdate(fixedDeltaTime);
    }
}

void SystemRegistry::RenderAll() {
    for (auto& system : m_Systems) {
        system->Render();
    }
}

void SystemRegistry::EditorGUIAll() {
    for (auto& system : m_Systems) {
        system->EditorGUI();
    }
}
