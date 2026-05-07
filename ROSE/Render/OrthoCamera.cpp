#include "OrthoCamera.h"
#include <glm/gtc/matrix_transform.hpp>

OrthoCamera::OrthoCamera() {
    UpdateMatrices();
}

OrthoCamera::OrthoCamera(float left, float right, float bottom, float top)
    : m_Left(left), m_Right(right), m_Bottom(bottom), m_Top(top) {
    UpdateMatrices();
}

void OrthoCamera::SetProjection(float left, float right, float bottom, float top) {
    m_Left = left;
    m_Right = right;
    m_Bottom = bottom;
    m_Top = top;
    m_NeedsUpdate = true;
    UpdateMatrices();
}

void OrthoCamera::SetProjection(float width, float height) {
    SetOrthoBounds(width, height);
}

void OrthoCamera::SetAspect(float width, float height) {
    SetOrthoBounds(width, height);
}

glm::mat4 OrthoCamera::GetViewMatrix() const {
    return m_ViewMatrix;
}

glm::mat4 OrthoCamera::GetProjectionMatrix() const {
    return m_ProjectionMatrix;
}

glm::mat4 OrthoCamera::GetViewProjectionMatrix() const {
    return m_ViewProjectionMatrix;
}

void OrthoCamera::SetPosition(const glm::vec3& pos) {
    if (m_Position != pos) {
        m_Position = pos;
        m_NeedsUpdate = true;
        UpdateMatrices();
    }
}

void OrthoCamera::SetRotation(float angle) {
    if (m_Rotation != angle) {
        m_Rotation = angle;
        m_NeedsUpdate = true;
        UpdateMatrices();
    }
}

void OrthoCamera::SetZoom(float zoom) {
    if (m_Zoom != zoom && zoom > 0.01f) {
        m_Zoom = zoom;
        m_NeedsUpdate = true;
        UpdateMatrices();
    }
}

void OrthoCamera::SetLeft(float left) {
    m_Left = left;
    m_NeedsUpdate = true;
    UpdateMatrices();
}

void OrthoCamera::SetRight(float right) {
    m_Right = right;
    m_NeedsUpdate = true;
    UpdateMatrices();
}

void OrthoCamera::SetBottom(float bottom) {
    m_Bottom = bottom;
    m_NeedsUpdate = true;
    UpdateMatrices();
}

void OrthoCamera::SetTop(float top) {
    m_Top = top;
    m_NeedsUpdate = true;
    UpdateMatrices();
}

void OrthoCamera::SetOrthoBounds(float left, float right, float bottom, float top) {
    m_Left = left;
    m_Right = right;
    m_Bottom = bottom;
    m_Top = top;
    m_NeedsUpdate = true;
    UpdateMatrices();
}

void OrthoCamera::SetOrthoBounds(float width, float height) {
    float halfW = width * 0.5f / m_Zoom;
    float halfH = height * 0.5f / m_Zoom;
    m_Left = -halfW;
    m_Right = halfW;
    m_Bottom = -halfH;
    m_Top = halfH;
    m_NeedsUpdate = true;
    UpdateMatrices();
}

glm::vec2 OrthoCamera::ScreenToWorld(const glm::vec2& screenPos, int screenWidth, int screenHeight) const {
    float x = (screenPos.x / screenWidth) * (m_Right - m_Left) + m_Left;
    float y = (1.0f - screenPos.y / screenHeight) * (m_Top - m_Bottom) + m_Bottom;
    return glm::vec2(x, y) + glm::vec2(m_Position.x, m_Position.y);
}

glm::vec2 OrthoCamera::WorldToScreen(const glm::vec2& worldPos, int screenWidth, int screenHeight) const {
    float x = ((worldPos.x - m_Position.x - m_Left) / (m_Right - m_Left)) * screenWidth;
    float y = (1.0f - ((worldPos.y - m_Position.y - m_Bottom) / (m_Top - m_Bottom))) * screenHeight;
    return glm::vec2(x, y);
}

void OrthoCamera::UpdateMatrices() {
    if (!m_NeedsUpdate) return;

    m_ProjectionMatrix = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, -1.0f, 1.0f);

    m_ViewMatrix = glm::mat4(1.0f);
    m_ViewMatrix = glm::translate(m_ViewMatrix, glm::vec3(-m_Position.x, -m_Position.y, 0.0f));
    m_ViewMatrix = glm::rotate(m_ViewMatrix, glm::radians(m_Rotation), glm::vec3(0.0f, 0.0f, 1.0f));

    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

    m_NeedsUpdate = false;
}
