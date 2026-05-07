#ifndef ENTITY_H
#define ENTITY_H

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <algorithm>

using EntityID = uint32_t;
const EntityID INVALID_ENTITY = 0xFFFFFFFF;

class ComponentBase {
public:
    virtual ~ComponentBase() = default;
};

class EntityRegistry {
public:
    static EntityRegistry& Instance();

    EntityID CreateEntity();
    void DestroyEntity(EntityID id);
    bool IsAlive(EntityID id) const;

    template<typename T, typename... Args>
    T& AddComponent(EntityID id, Args&&... args) {
        auto key = std::type_index(typeid(T));
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *component;
        m_Components[key][id] = std::move(component);
        return ref;
    }

    template<typename T>
    T& GetComponent(EntityID id) {
        auto key = std::type_index(typeid(T));
        return *static_cast<T*>(m_Components[key][id].get());
    }

    template<typename T>
    bool HasComponent(EntityID id) const {
        auto key = std::type_index(typeid(T));
        auto it = m_Components.find(key);
        if (it == m_Components.end()) return false;
        return it->second.find(id) != it->second.end();
    }

    template<typename T>
    void RemoveComponent(EntityID id) {
        auto key = std::type_index(typeid(T));
        auto it = m_Components.find(key);
        if (it != m_Components.end()) {
            it->second.erase(id);
        }
    }

    const std::vector<EntityID>& GetAllEntities() const { return m_Entities; }
    size_t GetEntityCount() const { return m_Entities.size(); }

private:
    EntityRegistry() = default;
    std::vector<EntityID> m_Entities;
    std::unordered_map<std::type_index, std::unordered_map<EntityID, std::unique_ptr<ComponentBase>>> m_Components;
    EntityID m_NextID = 0;
};

#endif
