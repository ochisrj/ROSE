#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <unordered_map>
#include <string>
#include <memory>
#include "../Texture.h"
#include "../ShaderClass.h"

class ResourceManager {
public:
    static ResourceManager& Instance();

    Texture* LoadTexture(const std::string& name, const char* path,
                         GLenum texType, GLenum slot, GLenum format, GLenum pixelType);
    Texture* GetTexture(const std::string& name);

    Shader* LoadShader(const std::string& name, const char* vertexFile, const char* fragmentFile);
    Shader* GetShader(const std::string& name);

    void Clear();

private:
    ResourceManager() = default;
    std::unordered_map<std::string, std::unique_ptr<Texture>> m_Textures;
    std::unordered_map<std::string, std::unique_ptr<Shader>> m_Shaders;
};

#endif
