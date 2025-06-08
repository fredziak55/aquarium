#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Camera movement directions
const int FORWARD = 0;
const int BACKWARD = 1;
const int LEFT = 2;
const int RIGHT = 3;

class Camera {
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), 
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
           float yaw = -90.0f, float pitch = 0.0f);
    
    glm::mat4 GetViewMatrix();
    void ProcessKeyboard(int direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ProcessMouseScroll(float yoffset);
    
    // Camera attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    
    // Euler angles
    float Yaw;
    float Pitch;
    
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    
private:
    void updateCameraVectors();

    // New members for smoothing mouse movement
    float smoothedXOffset = 0.0f;
    float smoothedYOffset = 0.0f;
    float smoothingFactor = 0.1f; // Adjust between 0 (no smoothing) and 1 (full smoothing)
};

#endif