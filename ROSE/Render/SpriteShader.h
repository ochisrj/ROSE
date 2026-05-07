#ifndef SPRITE_SHADER_H
#define SPRITE_SHADER_H

#include <string>
#include <vector>
#include <glm/glm.hpp>

enum class SpriteShaderType {
    Default2D,
    NormalMap,
    Outline,
    Glow,
    Flash,
    Dissolve,
    ColorGrading,
    CRT
};

class SpriteShader {
public:
    static SpriteShader& Instance();

    void Init();
    void UseShader(SpriteShaderType type);
    unsigned int GetShaderID(SpriteShaderType type) const;

    void SetProjection(const glm::mat4& projection);
    void SetView(const glm::mat4& view);

    void SetColor(glm::vec4 color);
    void SetLightPos(glm::vec2 pos);
    void SetLightColor(glm::vec3 color);
    void SetLightIntensity(float intensity);
    void SetNormalMapEnabled(bool enabled);

    void SetOutlineColor(glm::vec3 color);
    void SetOutlineWidth(float width);

    void SetGlowColor(glm::vec3 color);
    void SetGlowIntensity(float intensity);

    void SetFlashIntensity(float intensity);
    void SetFlashColor(glm::vec3 color);

    void SetDissolveProgress(float progress);
    void SetDissolveColor(glm::vec3 color);
    void SetDissolveNoiseTexture(unsigned int id);

    void SetVignetteStrength(float strength);
    void SetVignetteColor(glm::vec3 color);

    void SetCRTScanlineStrength(float strength);
    void SetCRTCurvature(float curvature);

    void SetTextureUnit(int unit);
    void SetNormalMapUnit(int unit);

private:
    SpriteShader() = default;

    unsigned int CompileShader(const char* vertexSource, const char* fragmentSource);
    unsigned int GetUniformLocation(const char* name);

    unsigned int m_Shaders[(int)SpriteShaderType::CRT + 1] = {0};
    unsigned int m_CurrentShader = 0;

    std::string ReadFile(const std::string& path);
    std::string GetShaderDirectory() const;
};

#endif
