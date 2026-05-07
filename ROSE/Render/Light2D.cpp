#include <glad/glad.h>
#include "Light2D.h"
#include "../Core/Log.h"

LightManager& LightManager::Instance() {
    static LightManager instance;
    return instance;
}

void LightManager::Init() {
    m_Lights.reserve(MAX_LIGHTS);
    ROSE_LOG_INFO("LightManager initialized (max %d lights)", MAX_LIGHTS);
}

unsigned int LightManager::CreateLight(const Light2D& light) {
    if (m_Lights.size() >= MAX_LIGHTS) {
        ROSE_LOG_WARN("Maximum lights reached (%d)", MAX_LIGHTS);
        return 0;
    }

    Light2D l = light;
    l.ID = ++m_NextID;
    m_Lights.push_back(l);
    return l.ID;
}

void LightManager::UpdateLight(unsigned int id, const Light2D& light) {
    for (auto& l : m_Lights) {
        if (l.ID == id) {
            l = light;
            l.ID = id;
            return;
        }
    }
}

void LightManager::DeleteLight(unsigned int id) {
    m_Lights.erase(
        std::remove_if(m_Lights.begin(), m_Lights.end(), [id](const Light2D& l) {
            return l.ID == id;
        }),
        m_Lights.end()
    );
}

void LightManager::SetLightEnabled(unsigned int id, bool enabled) {
    for (auto& l : m_Lights) {
        if (l.ID == id) {
            l.Enabled = enabled;
            return;
        }
    }
}

void LightManager::SetAmbientColor(glm::vec3 color) {
    m_AmbientColor = color;
}

void LightManager::SetAmbientIntensity(float intensity) {
    m_AmbientIntensity = intensity;
}

void LightManager::UploadLightsToShader(unsigned int shaderID) {
    glUseProgram(shaderID);

    glUniform3f(glGetUniformLocation(shaderID, "u_AmbientColor"),
                m_AmbientColor.x, m_AmbientColor.y, m_AmbientColor.z);
    glUniform1f(glGetUniformLocation(shaderID, "u_LightIntensity"), m_AmbientIntensity);

    int lightCount = 0;
    for (const auto& light : m_Lights) {
        if (!light.Enabled) continue;
        if (lightCount >= MAX_LIGHTS) break;

        std::string prefix = "u_Lights[" + std::to_string(lightCount) + "].";

        glUniform3f(glGetUniformLocation(shaderID, (prefix + "Position").c_str()),
                    light.Position.x, light.Position.y, light.Position.z);
        glUniform3f(glGetUniformLocation(shaderID, (prefix + "Color").c_str()),
                    light.Color.x, light.Color.y, light.Color.z);
        glUniform1f(glGetUniformLocation(shaderID, (prefix + "Intensity").c_str()),
                    light.Intensity);
        glUniform1f(glGetUniformLocation(shaderID, (prefix + "Range").c_str()),
                    light.Range);

        if (light.Type == LightType::Spot) {
            glUniform3f(glGetUniformLocation(shaderID, (prefix + "Direction").c_str()),
                        light.Direction.x, light.Direction.y, light.Direction.z);
            glUniform1f(glGetUniformLocation(shaderID, (prefix + "InnerAngle").c_str()),
                        glm::radians(light.InnerAngle));
            glUniform1f(glGetUniformLocation(shaderID, (prefix + "OuterAngle").c_str()),
                        glm::radians(light.OuterAngle));
        }

        glUniform1i(glGetUniformLocation(shaderID, (prefix + "Type").c_str()),
                    (int)light.Type);

        lightCount++;
    }

    glUniform1i(glGetUniformLocation(shaderID, "u_LightCount"), lightCount);
}

void LightManager::Clear() {
    m_Lights.clear();
    m_NextID = 0;
}
