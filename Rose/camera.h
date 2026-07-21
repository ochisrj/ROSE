#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// ทิศทางการเคลื่อนที่ของกล้อง
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// ค่าเริ่มต้นของกล้อง
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
    // Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // Euler Angles
    float Yaw;
    float Pitch;

    // Options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // Constructor แบบใช้ Vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW,
        float pitch = PITCH);

    // คำนวณ View Matrix ส่งให้ Shader
    glm::mat4 GetViewMatrix();

    // ประมวลผลปุ่มกด (WASD / Space / Shift)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    // ประมวลผลการขยับเมาส์
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // ประมวลผลการหมุนลูกกลิ้งเมาส์ (Zoom/FOV)
    void ProcessMouseScroll(float yoffset);

private:
    // คำนวณ Vector Front/Right/Up จากค่า Yaw และ Pitch
    void updateCameraVectors();
};

#endif