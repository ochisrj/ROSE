#ifndef POST_PROCESS_H
#define POST_PROCESS_H

#include <vector>
#include <string>

enum class PostProcessEffect {
    Bloom,
    ColorGrading,
    Vignette,
    CRT,
    ChromaticAberration,
    Pixelate,
    Blur,
    Sharpen,
    Sepia,
    Grayscale
};

struct PostProcessSettings {
    bool BloomEnabled = false;
    float BloomIntensity = 0.8f;
    float BloomThreshold = 0.5f;
    int BloomBlurIterations = 4;

    bool VignetteEnabled = true;
    float VignetteStrength = 0.4f;
    float VignetteSmoothness = 0.5f;

    bool CRTEnabled = false;
    float CRTScanlineStrength = 0.3f;
    float CRTCurvature = 0.02f;
    float CRTNoiseAmount = 0.05f;

    bool ColorGradingEnabled = false;
    float Brightness = 1.0f;
    float Contrast = 1.0f;
    float Saturation = 1.0f;

    bool PixelateEnabled = false;
    float PixelSize = 4.0f;
};

class PostProcess {
public:
    static PostProcess& Instance();

    void Init(int width, int height);
    void Resize(int width, int height);

    void BeginRender();
    void EndRender();
    void ApplyEffects();

    void AddEffect(PostProcessEffect effect);
    void RemoveEffect(PostProcessEffect effect);
    void ClearEffects();

    PostProcessSettings& GetSettings() { return m_Settings; }
    const PostProcessSettings& GetSettings() const { return m_Settings; }

    unsigned int GetSceneTextureID() const { return m_SceneTextureID; }

    void Shutdown();

private:
    PostProcess() = default;

    void CreateFramebuffer();
    void DeleteFramebuffer();
    void CreateQuad();
    void BindFramebuffer();
    void UnbindFramebuffer();
    void RenderQuad(unsigned int shaderID);

    unsigned int m_FramebufferID = 0;
    unsigned int m_SceneTextureID = 0;
    unsigned int m_RenderbufferID = 0;
    unsigned int m_QuadVAO = 0;
    unsigned int m_QuadVBO = 0;

    std::vector<PostProcessEffect> m_ActiveEffects;
    PostProcessSettings m_Settings;
    int m_Width = 1920;
    int m_Height = 1080;
    bool m_Initialized = false;
};

#endif
