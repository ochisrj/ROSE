#include <glad/glad.h>
#include "TilemapRenderer.h"
#include "../Core/Log.h"
#include "../Texture.h"
#include "../Resource/ResourceManager.h"

TilemapRenderer::TilemapRenderer() {
}

TilemapRenderer::~TilemapRenderer() {
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_EBO) glDeleteBuffers(1, &m_EBO);
}

void TilemapRenderer::Init() {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(4 * sizeof(float)));

    glBindVertexArray(0);

    ROSE_LOG_INFO("TilemapRenderer initialized");
}

void TilemapRenderer::LoadTileset(const std::string& name, const std::string& path,
                                   int tileWidth, int tileHeight) {
    auto* tex = ResourceManager::Instance().LoadTexture(name, path.c_str(),
                                                         GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    m_Tilesets[name] = tex->ID;
    m_TileSizes[name] = tileWidth * 10000 + tileHeight;
    ROSE_LOG_INFO("Loaded tileset '%s' (%dx%d tiles)", name.c_str(), tileWidth, tileHeight);
}

void TilemapRenderer::CreateTilemap(const std::string& name, int width, int height,
                                     int tileWidth, int tileHeight,
                                     TilemapOrientation orientation) {
    m_Tilemap.Width = width;
    m_Tilemap.Height = height;
    m_Tilemap.TileWidth = tileWidth;
    m_Tilemap.TileHeight = tileHeight;
    m_Tilemap.Orientation = orientation;
    m_Tilemap.Tiles.assign(width * height, -1);
    m_NeedsRebuild = true;

    ROSE_LOG_INFO("Created tilemap '%s' (%dx%d)", name.c_str(), width, height);
}

void TilemapRenderer::SetTile(int x, int y, int tileIndex) {
    if (x < 0 || x >= m_Tilemap.Width || y < 0 || y >= m_Tilemap.Height) return;
    m_Tilemap.Tiles[y * m_Tilemap.Width + x] = tileIndex;
    m_NeedsRebuild = true;
}

int TilemapRenderer::GetTile(int x, int y) const {
    if (x < 0 || x >= m_Tilemap.Width || y < 0 || y >= m_Tilemap.Height) return -1;
    return m_Tilemap.Tiles[y * m_Tilemap.Width + x];
}

void TilemapRenderer::SetTileColor(int x, int y, glm::vec4 color) {
    (void)x; (void)y; (void)color;
}

void TilemapRenderer::SetTileCollision(int x, int y, bool collidable) {
    (void)x; (void)y; (void)collidable;
}

glm::vec2 TilemapRenderer::GetTilePosition(int x, int y) const {
    if (m_Tilemap.Orientation == TilemapOrientation::Isometric) {
        return IsoToScreen(x, y);
    }
    return glm::vec2(
        (float)x * m_Tilemap.TileWidth,
        (float)y * m_Tilemap.TileHeight
    );
}

glm::vec2 TilemapRenderer::WorldToTile(glm::vec2 worldPos) const {
    if (m_Tilemap.Orientation == TilemapOrientation::Isometric) {
        float halfW = m_Tilemap.TileWidth * 0.5f;
        float halfH = m_Tilemap.TileHeight * 0.5f;
        int tileX = (int)((worldPos.x / halfW + worldPos.y / halfH) * 0.5f);
        int tileY = (int)((worldPos.y / halfH - worldPos.x / halfW) * 0.5f);
        return glm::vec2((float)tileX, (float)tileY);
    }
    return glm::vec2(
        floor(worldPos.x / m_Tilemap.TileWidth),
        floor(worldPos.y / m_Tilemap.TileHeight)
    );
}

bool TilemapRenderer::CheckCollision(glm::vec2 position, glm::vec2 size) const {
    glm::vec2 tileMin = WorldToTile(position);
    glm::vec2 tileMax = WorldToTile(position + size);

    for (int y = (int)tileMin.y; y <= (int)tileMax.y; y++) {
        for (int x = (int)tileMin.x; x <= (int)tileMax.x; x++) {
            int tile = GetTile(x, y);
            if (tile >= 0) return true;
        }
    }
    return false;
}

void TilemapRenderer::Submit() {
    if (m_NeedsRebuild) {
        BuildVertexData();
        m_NeedsRebuild = false;
    }
    Render();
}

void TilemapRenderer::Render() {
    if (!m_Visible || m_Tilemap.Width == 0 || m_Tilemap.Height == 0) return;

    if (m_VertexData.empty()) return;

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_VertexData.size() * sizeof(float), m_VertexData.data());

    glDrawElements(GL_TRIANGLES, (GLsizei)m_IndexData.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void TilemapRenderer::BuildVertexData() {
    m_VertexData.clear();
    m_IndexData.clear();

    if (m_Tilemap.Orientation == TilemapOrientation::Isometric) {
        BuildIsometricVertexData();
    } else {
        BuildOrthogonalVertexData();
    }
}

void TilemapRenderer::BuildOrthogonalVertexData() {
    int tw = m_Tilemap.TileWidth;
    int th = m_Tilemap.TileHeight;

    for (int y = 0; y < m_Tilemap.Height; y++) {
        for (int x = 0; x < m_Tilemap.Width; x++) {
            int tileIndex = m_Tilemap.Tiles[y * m_Tilemap.Width + x];
            if (tileIndex < 0) continue;

            float px = (float)x * tw;
            float py = (float)y * th;

            m_VertexData.push_back(px);
            m_VertexData.push_back(py);
            m_VertexData.push_back(0.0f);
            m_VertexData.push_back(0.0f);
            m_VertexData.push_back(1.0f);
            m_VertexData.push_back(0.0f);

            m_VertexData.push_back(px + tw);
            m_VertexData.push_back(py);
            m_VertexData.push_back(1.0f);
            m_VertexData.push_back(0.0f);
            m_VertexData.push_back(1.0f);
            m_VertexData.push_back(0.0f);

            m_VertexData.push_back(px + tw);
            m_VertexData.push_back(py + th);
            m_VertexData.push_back(1.0f);
            m_VertexData.push_back(1.0f);
            m_VertexData.push_back(1.0f);
            m_VertexData.push_back(0.0f);

            m_VertexData.push_back(px);
            m_VertexData.push_back(py + th);
            m_VertexData.push_back(0.0f);
            m_VertexData.push_back(1.0f);
            m_VertexData.push_back(1.0f);
            m_VertexData.push_back(0.0f);

            unsigned int base = (unsigned int)(m_IndexData.size() / 6) * 4;
            m_IndexData.push_back(base + 0);
            m_IndexData.push_back(base + 1);
            m_IndexData.push_back(base + 2);
            m_IndexData.push_back(base + 2);
            m_IndexData.push_back(base + 3);
            m_IndexData.push_back(base + 0);
        }
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndexData.size() * sizeof(int), m_IndexData.data(), GL_DYNAMIC_DRAW);
}

void TilemapRenderer::BuildIsometricVertexData() {
    int tw = m_Tilemap.TileWidth;
    int th = m_Tilemap.TileHeight;

    for (int y = 0; y < m_Tilemap.Height; y++) {
        for (int x = 0; x < m_Tilemap.Width; x++) {
            int tileIndex = m_Tilemap.Tiles[y * m_Tilemap.Width + x];
            if (tileIndex < 0) continue;

            glm::vec2 pos = IsoToScreen(x, y);
            float hw = tw * 0.5f;
            float hh = th * 0.5f;

            m_VertexData.push_back(pos.x);
            m_VertexData.push_back(pos.y - hh);
            m_VertexData.push_back(0.5f);
            m_VertexData.push_back(0.0f);
            m_VertexData.push_back(1.0f);
            m_VertexData.push_back(0.0f);

            m_VertexData.push_back(pos.x + hw);
            m_VertexData.push_back(pos.y);
            m_VertexData.push_back(1.0f);
            m_VertexData.push_back(0.5f);
            m_VertexData.push_back(1.0f);
            m_VertexData.push_back(0.0f);

            m_VertexData.push_back(pos.x);
            m_VertexData.push_back(pos.y + hh);
            m_VertexData.push_back(0.5f);
            m_VertexData.push_back(1.0f);
            m_VertexData.push_back(1.0f);
            m_VertexData.push_back(0.0f);

            m_VertexData.push_back(pos.x - hw);
            m_VertexData.push_back(pos.y);
            m_VertexData.push_back(0.0f);
            m_VertexData.push_back(0.5f);
            m_VertexData.push_back(1.0f);
            m_VertexData.push_back(0.0f);

            unsigned int base = (unsigned int)(m_IndexData.size() / 6) * 4;
            m_IndexData.push_back(base + 0);
            m_IndexData.push_back(base + 1);
            m_IndexData.push_back(base + 2);
            m_IndexData.push_back(base + 2);
            m_IndexData.push_back(base + 3);
            m_IndexData.push_back(base + 0);
        }
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndexData.size() * sizeof(int), m_IndexData.data(), GL_DYNAMIC_DRAW);
}

glm::vec2 TilemapRenderer::IsoToScreen(int x, int y) const {
    float hw = m_Tilemap.TileWidth * 0.5f;
    float hh = m_Tilemap.TileHeight * 0.5f;
    return glm::vec2(
        (x - y) * hw,
        (x + y) * hh
    );
}

void TilemapRenderer::SetAutoTileRules(int ruleIndex, const std::vector<int>& tiles) {
    while ((int)m_AutoTileRules.size() <= ruleIndex) {
        m_AutoTileRules.push_back({});
    }
    m_AutoTileRules[ruleIndex].Tiles = tiles;
}

void TilemapRenderer::ApplyAutoTile(int x, int y, int ruleIndex) {
    if (ruleIndex < 0 || ruleIndex >= (int)m_AutoTileRules.size()) return;

    bool top = GetTile(x, y - 1) >= 0;
    bool right = GetTile(x + 1, y) >= 0;
    bool bottom = GetTile(x, y + 1) >= 0;
    bool left = GetTile(x - 1, y) >= 0;

    int index = (top ? 1 : 0) | (right ? 2 : 0) | (bottom ? 4 : 0) | (left ? 8 : 0);

    if (index < (int)m_AutoTileRules[ruleIndex].Tiles.size()) {
        SetTile(x, y, m_AutoTileRules[ruleIndex].Tiles[index]);
    }
}
