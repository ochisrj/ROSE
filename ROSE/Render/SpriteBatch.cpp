#include <glad/glad.h>
#include "SpriteBatch.h"
#include "SpriteShader.h"
#include "../Core/Log.h"

SpriteBatch::SpriteBatch() {
}

SpriteBatch::~SpriteBatch() {
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_EBO) glDeleteBuffers(1, &m_EBO);
}

void SpriteBatch::Init() {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_SPRITE_BATCH_SIZE * 4 * sizeof(SpriteVertex), nullptr, GL_DYNAMIC_DRAW);

    InitQuadIndices();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_SPRITE_BATCH_SIZE * 6 * sizeof(unsigned int), m_Indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);

    m_Vertices.resize(MAX_SPRITE_BATCH_SIZE * 4);
    m_TextureSlots.reserve(MAX_TEXTURES_IN_BATCH);

    m_ShaderID = SpriteShader::Instance().GetShaderID(SpriteShaderType::Default2D);

    ROSE_LOG_INFO("SpriteBatch initialized (max %d sprites, %d textures per batch)", MAX_SPRITE_BATCH_SIZE, MAX_TEXTURES_IN_BATCH);
}

void SpriteBatch::InitQuadIndices() {
    unsigned int offset = 0;
    for (int i = 0; i < MAX_SPRITE_BATCH_SIZE; i++) {
        m_Indices.push_back(offset + 0);
        m_Indices.push_back(offset + 1);
        m_Indices.push_back(offset + 2);
        m_Indices.push_back(offset + 2);
        m_Indices.push_back(offset + 3);
        m_Indices.push_back(offset + 0);
        offset += 4;
    }
}

void SpriteBatch::Begin() {
    m_IsRendering = true;
    m_SpriteCount = 0;
    m_TextureSlots.clear();
    m_TextureSlots.push_back(0);
    m_TexturesInBatch = 1;
}

void SpriteBatch::End() {
    m_IsRendering = false;
    Flush();
}

void SpriteBatch::Flush() {
    if (m_SpriteCount == 0) return;

    glUseProgram(m_ShaderID);

    int textureSlots[MAX_TEXTURES_IN_BATCH];
    for (unsigned int i = 0; i < m_TexturesInBatch; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_TextureSlots[i]);
        textureSlots[i] = i;
    }

    GLint location = glGetUniformLocation(m_ShaderID, "u_Textures");
    if (location != -1) {
        glUniform1iv(location, m_TexturesInBatch, textureSlots);
    }

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_SpriteCount * 4 * sizeof(SpriteVertex), m_Vertices.data());

    glDrawElements(GL_TRIANGLES, m_SpriteCount * 6, GL_UNSIGNED_INT, 0);
    m_DrawCalls++;

    glBindVertexArray(0);

    m_SpriteCount = 0;
    m_TextureSlots.clear();
    m_TextureSlots.push_back(0);
    m_TexturesInBatch = 1;
}

int SpriteBatch::GetTextureSlot(unsigned int textureID) {
    for (unsigned int i = 0; i < m_TextureSlots.size(); i++) {
        if (m_TextureSlots[i] == textureID) {
            return (int)i;
        }
    }

    if (m_TextureSlots.size() >= MAX_TEXTURES_IN_BATCH) {
        FlushBatch();
    }

    m_TextureSlots.push_back(textureID);
    return (int)(m_TextureSlots.size() - 1);
}

void SpriteBatch::DrawSprite(const SpriteData& sprite) {
    if (m_SpriteCount >= MAX_SPRITE_BATCH_SIZE) {
        FlushBatch();
    }

    int textureSlot = GetTextureSlot(sprite.TextureID);

    glm::vec2 halfSize = sprite.Size * 0.5f;
    glm::vec2 positions[4] = {
        glm::vec2(-halfSize.x, -halfSize.y),
        glm::vec2( halfSize.x, -halfSize.y),
        glm::vec2( halfSize.x,  halfSize.y),
        glm::vec2(-halfSize.x,  halfSize.y)
    };

    for (int i = 0; i < 4; i++) {
        SpriteVertex& v = m_Vertices[m_SpriteCount * 4 + i];
        v.Position = sprite.Position + positions[i];
        v.TexCoord = sprite.TexCoords[i];
        v.TextureSlot = (float)textureSlot;
        v.DepthSortY = sprite.DepthSortY;
    }

    m_SpriteCount++;
}

void SpriteBatch::DrawSprite(glm::vec2 position, glm::vec2 size, unsigned int textureID,
                              glm::vec4 color, float depthSortY, unsigned int normalMapID) {
    SpriteData data;
    data.Position = position;
    data.Size = size;
    data.Color = color;
    data.TextureID = textureID;
    data.DepthSortY = depthSortY;
    data.TexCoords[0] = glm::vec2(0.0f, 0.0f);
    data.TexCoords[1] = glm::vec2(1.0f, 0.0f);
    data.TexCoords[2] = glm::vec2(1.0f, 1.0f);
    data.TexCoords[3] = glm::vec2(0.0f, 1.0f);
    data.NormalMapID = normalMapID;

    DrawSprite(data);
}

void SpriteBatch::DrawSpriteRotated(glm::vec2 position, glm::vec2 size, float rotation,
                                     unsigned int textureID, glm::vec4 color, float depthSortY) {
    if (m_SpriteCount >= MAX_SPRITE_BATCH_SIZE) {
        FlushBatch();
    }

    int textureSlot = GetTextureSlot(textureID);

    glm::vec2 halfSize = size * 0.5f;
    glm::vec2 positions[4] = {
        glm::vec2(-halfSize.x, -halfSize.y),
        glm::vec2( halfSize.x, -halfSize.y),
        glm::vec2( halfSize.x,  halfSize.y),
        glm::vec2(-halfSize.x,  halfSize.y)
    };

    float cosA = cos(rotation);
    float sinA = sin(rotation);

    for (int i = 0; i < 4; i++) {
        glm::vec2 rotated = glm::vec2(
            positions[i].x * cosA - positions[i].y * sinA,
            positions[i].x * sinA + positions[i].y * cosA
        );

        SpriteVertex& v = m_Vertices[m_SpriteCount * 4 + i];
        v.Position = position + rotated;
        v.TexCoord = glm::vec2((float)(i % 2), (float)(i / 2));
        v.TextureSlot = (float)textureSlot;
        v.DepthSortY = depthSortY;
    }

    m_SpriteCount++;
}

void SpriteBatch::Submit() {
    Flush();
}

void SpriteBatch::FlushBatch() {
    Flush();
}
