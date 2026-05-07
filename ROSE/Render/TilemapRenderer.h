#ifndef TILEMAP_RENDERER_H
#define TILEMAP_RENDERER_H

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <unordered_map>
#include "Renderer.h"

enum class TilemapOrientation {
    Orthogonal,
    Isometric,
    Staggered
};

struct TileInfo {
    int X, Y;
    unsigned int TextureID;
    glm::vec2 TexCoords[4];
    glm::vec4 Color;
    float Depth;
    bool Collidable;
};

struct TilemapData {
    std::vector<int> Tiles;
    int Width = 0;
    int Height = 0;
    int TileWidth = 16;
    int TileHeight = 16;
    TilemapOrientation Orientation = TilemapOrientation::Orthogonal;
};

class TilemapRenderer : public Renderable {
public:
    TilemapRenderer();
    ~TilemapRenderer();

    void Init();

    void LoadTileset(const std::string& name, const std::string& path,
                     int tileWidth, int tileHeight);
    void CreateTilemap(const std::string& name, int width, int height,
                       int tileWidth, int tileHeight,
                       TilemapOrientation orientation = TilemapOrientation::Orthogonal);

    void SetTile(int x, int y, int tileIndex);
    int GetTile(int x, int y) const;

    void SetTileColor(int x, int y, glm::vec4 color);
    void SetTileCollision(int x, int y, bool collidable);

    glm::vec2 GetTilePosition(int x, int y) const;
    glm::vec2 WorldToTile(glm::vec2 worldPos) const;

    bool CheckCollision(glm::vec2 position, glm::vec2 size) const;

    virtual void Submit() override;
    virtual RenderLayer GetLayer() const override { return RenderLayer::Tilemap; }
    virtual float GetDepthSortY() const override { return 0.0f; }

    void Render();

    int GetWidth() const { return m_Tilemap.Width; }
    int GetHeight() const { return m_Tilemap.Height; }

    void SetVisible(bool visible) { m_Visible = visible; }
    bool IsVisible() const { return m_Visible; }

    void SetAutoTileRules(int ruleIndex, const std::vector<int>& tiles);
    void ApplyAutoTile(int x, int y, int ruleIndex);

private:
    void BuildVertexData();
    void BuildOrthogonalVertexData();
    void BuildIsometricVertexData();
    void UploadVertexData();
    glm::vec2 IsoToScreen(int x, int y) const;

    std::unordered_map<std::string, unsigned int> m_Tilesets;
    std::unordered_map<std::string, int> m_TileSizes;

    TilemapData m_Tilemap;
    std::vector<float> m_VertexData;
    std::vector<int> m_IndexData;

    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_EBO = 0;
    unsigned int m_ShaderID = 0;

    bool m_Visible = true;
    bool m_NeedsRebuild = false;

    struct AutoTileRule {
        std::vector<int> Tiles;
    };
    std::vector<AutoTileRule> m_AutoTileRules;
};

#endif
