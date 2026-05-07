#ifndef ORTHO_CAMERA_H
#define ORTHO_CAMERA_H

#include <glm/glm.hpp>

class OrthoCamera {
public:
    static OrthoCamera& Instance() {
        static OrthoCamera instance;
        return instance;
    }

    OrthoCamera();
    OrthoCamera(float left, float right, float bottom, float top);

    void SetProjection(float left, float right, float bottom, float top);
    void SetProjection(float width, float height);
    void SetAspect(float width, float height);

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetViewProjectionMatrix() const;

    void SetPosition(const glm::vec3& pos);
    glm::vec3 GetPosition() const { return m_Position; }

    void SetRotation(float angle);
    float GetRotation() const { return m_Rotation; }

    void SetZoom(float zoom);
    float GetZoom() const { return m_Zoom; }

    void SetLeft(float left);
    void SetRight(float right);
    void SetBottom(float bottom);
    void SetTop(float top);

    void SetOrthoBounds(float left, float right, float bottom, float top);
    void SetOrthoBounds(float width, float height);

    glm::vec2 ScreenToWorld(const glm::vec2& screenPos, int screenWidth, int screenHeight) const;
    glm::vec2 WorldToScreen(const glm::vec2& worldPos, int screenWidth, int screenHeight) const;

    void UpdateMatrices();

private:
    glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
    float m_Rotation = 0.0f;
    float m_Zoom = 1.0f;

    float m_Left = -960.0f;
    float m_Right = 960.0f;
    float m_Bottom = -540.0f;
    float m_Top = 540.0f;

    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ViewProjectionMatrix;
    bool m_NeedsUpdate = true;
};

#endif
