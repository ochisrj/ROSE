#ifndef SPRITE_BATCH_H
#define SPRITE_BATCH_H

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include "Renderer.h"

const int MAX_SPRITE_BATCH_SIZE = 10000;
const int MAX_TEXTURES_IN_BATCH = 32;

struct SpriteVertex {
    glm::vec2 Position;
    glm::vec2 TexCoord;
    float TextureSlot;
    float DepthSortY;
};

struct SpriteData {
    glm::vec2 Position;
    glm::vec2 Size;
    glm::vec4 Color;
    glm::vec2 TexCoords[4];
    float DepthSortY;
    unsigned int TextureID;
    float Layer;
    bool HasNormalMap;
    unsigned int NormalMapID;
    bool IsTinted;
};

class SpriteBatch : public Renderable {
public:
    static SpriteBatch& Instance() {
        static SpriteBatch instance;
        return instance;
    }

    SpriteBatch();
    ~SpriteBatch();

    void Init();
    void Begin();
    void End();
    void Flush();

    void DrawSprite(const SpriteData& sprite);
    void DrawSprite(glm::vec2 position, glm::vec2 size, unsigned int textureID,
                    glm::vec4 color = glm::vec4(1.0f), float depthSortY = 0.0f,
                    unsigned int normalMapID = 0);
    void DrawSpriteRotated(glm::vec2 position, glm::vec2 size, float rotation,
                           unsigned int textureID, glm::vec4 color = glm::vec4(1.0f),
                           float depthSortY = 0.0f);

    virtual void Submit() override;
    virtual RenderLayer GetLayer() const override { return RenderLayer::SpriteForeground; }
    virtual float GetDepthSortY() const override { return 0.0f; }

    unsigned int GetSpriteCount() const { return m_SpriteCount; }
    unsigned int GetDrawCalls() const { return m_DrawCalls; }

    void SetShaderID(unsigned int id) { m_ShaderID = id; }
    unsigned int GetShaderID() const { return m_ShaderID; }

private:
    void InitQuadIndices();
    void InitQuadVertices();
    void FlushBatch();
    int GetTextureSlot(unsigned int textureID);

    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_EBO = 0;
    unsigned int m_ShaderID = 0;

    std::vector<SpriteVertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    unsigned int m_SpriteCount = 0;
    unsigned int m_DrawCalls = 0;
    unsigned int m_TexturesInBatch = 0;
    std::vector<unsigned int> m_TextureSlots;

    bool m_IsRendering = false;
};

#endif
