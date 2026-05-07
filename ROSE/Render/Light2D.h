#ifndef LIGHT_2D_H
#define LIGHT_2D_H

#include <glm/glm.hpp>
#include <vector>
#include <memory>

enum class LightType {
    Point,
    Spot,
    Directional,
    Ambient
};

struct Light2D {
    unsigned int ID = 0;
    LightType Type = LightType::Point;
    glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 Color = glm::vec3(1.0f, 1.0f, 1.0f);
    float Intensity = 1.0f;
    float Range = 10.0f;
    float InnerAngle = 0.0f;
    float OuterAngle = 45.0f;
    glm::vec3 Direction = glm::vec3(0.0f, 0.0f, 1.0f);
    bool Enabled = true;
};

struct NormalMap {
    unsigned int TextureID = 0;
    float Strength = 1.0f;
};

class LightManager {
public:
    static LightManager& Instance();

    void Init();

    unsigned int CreateLight(const Light2D& light);
    void UpdateLight(unsigned int id, const Light2D& light);
    void DeleteLight(unsigned int id);
    void SetLightEnabled(unsigned int id, bool enabled);

    const std::vector<Light2D>& GetActiveLights() const { return m_Lights; }
    int GetMaxLights() const { return MAX_LIGHTS; }

    void SetAmbientColor(glm::vec3 color);
    glm::vec3 GetAmbientColor() const { return m_AmbientColor; }
    void SetAmbientIntensity(float intensity);
    float GetAmbientIntensity() const { return m_AmbientIntensity; }

    void UploadLightsToShader(unsigned int shaderID);

    void Clear();

private:
    LightManager() = default;

    static constexpr int MAX_LIGHTS = 32;
    std::vector<Light2D> m_Lights;
    unsigned int m_NextID = 0;
    glm::vec3 m_AmbientColor = glm::vec3(0.1f, 0.1f, 0.15f);
    float m_AmbientIntensity = 0.3f;
};

#endif
