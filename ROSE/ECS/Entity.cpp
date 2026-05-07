#include "Entity.h"

EntityRegistry& EntityRegistry::Instance() {
    static EntityRegistry instance;
    return instance;
}

EntityID EntityRegistry::CreateEntity() {
    EntityID id = m_NextID++;
    m_Entities.push_back(id);
    return id;
}

void EntityRegistry::DestroyEntity(EntityID id) {
    m_Entities.erase(std::remove(m_Entities.begin(), m_Entities.end(), id), m_Entities.end());

    for (auto& [typeIndex, componentMap] : m_Components) {
        componentMap.erase(id);
    }
}

bool EntityRegistry::IsAlive(EntityID id) const {
    for (const auto& entityId : m_Entities) {
        if (entityId == id) return true;
    }
    return false;
}
