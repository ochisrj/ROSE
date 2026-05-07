#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <memory>
#include <string>

enum class RenderLayer {
    Background = 0,
    Tilemap = 1,
    SpriteForeground = 2,
    Effects = 3,
    UI = 4,
    Count = 5
};

enum class RenderMode {
    Orthographic2D,
    Perspective3D,
    HD2D
};

class Renderable {
public:
    virtual ~Renderable() = default;
    virtual void Submit() = 0;
    virtual RenderLayer GetLayer() const { return RenderLayer::SpriteForeground; }
    virtual float GetDepthSortY() const { return 0.0f; }
};

class Renderer {
public:
    static Renderer& Instance();

    void Init();
    void BeginFrame();
    void EndFrame();
    void DrawLayer(RenderLayer layer);
    void Clear();

    void Submit(Renderable* renderable);
    void SetRenderMode(RenderMode mode);
    RenderMode GetRenderMode() const { return m_Mode; }

    void SetBlendEnabled(bool enabled);
    bool IsBlendEnabled() const { return m_BlendEnabled; }

    void Resize(int width, int height);

private:
    Renderer() = default;

    std::vector<Renderable*> m_RenderQueue[(int)RenderLayer::Count];
    RenderMode m_Mode = RenderMode::Orthographic2D;
    bool m_BlendEnabled = true;
    int m_Width = 1920;
    int m_Height = 1080;
    bool m_Initialized = false;
};

#endif
