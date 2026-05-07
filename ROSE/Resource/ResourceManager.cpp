#include <glad/glad.h>
#include "ResourceManager.h"
#include "../Core/Log.h"

ResourceManager& ResourceManager::Instance() {
    static ResourceManager instance;
    return instance;
}

Texture* ResourceManager::LoadTexture(const std::string& name, const char* path,
                                       GLenum texType, GLenum slot, GLenum format, GLenum pixelType) {
    auto it = m_Textures.find(name);
    if (it != m_Textures.end()) {
        ROSE_LOG_DEBUG("Texture '%s' already loaded, returning cached", name.c_str());
        return it->second.get();
    }

    ROSE_LOG_INFO("Loading texture '%s' from '%s'", name.c_str(), path);
    auto texture = std::make_unique<Texture>(path, texType, slot, format, pixelType);
    Texture* ptr = texture.get();
    m_Textures[name] = std::move(texture);
    return ptr;
}

Texture* ResourceManager::GetTexture(const std::string& name) {
    auto it = m_Textures.find(name);
    return (it != m_Textures.end()) ? it->second.get() : nullptr;
}

Shader* ResourceManager::LoadShader(const std::string& name, const char* vertexFile, const char* fragmentFile) {
    auto it = m_Shaders.find(name);
    if (it != m_Shaders.end()) {
        ROSE_LOG_DEBUG("Shader '%s' already loaded, returning cached", name.c_str());
        return it->second.get();
    }

    ROSE_LOG_INFO("Loading shader '%s' from '%s' + '%s'", name.c_str(), vertexFile, fragmentFile);
    auto shader = std::make_unique<Shader>(vertexFile, fragmentFile);
    Shader* ptr = shader.get();
    m_Shaders[name] = std::move(shader);
    return ptr;
}

Shader* ResourceManager::GetShader(const std::string& name) {
    auto it = m_Shaders.find(name);
    return (it != m_Shaders.end()) ? it->second.get() : nullptr;
}

void ResourceManager::Clear() {
    ROSE_LOG_INFO("Clearing resource manager");
    m_Textures.clear();
    m_Shaders.clear();
}
