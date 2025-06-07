#include "Fish.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <cmath>

Fish::Fish(const char* modelPath, glm::vec3 startPos, glm::vec3 startDir, float speed) 
    : model(modelPath), position(startPos), direction(glm::normalize(startDir)), speed(speed), rotationAngle(0.0f) {}

void Fish::Update(float deltaTime, const glm::vec3& tankMin, const glm::vec3& tankMax) {
    // Move fish
    position += direction * speed * deltaTime;
    
    // Check boundaries and change direction if needed
    bool changeDir = false;
    for (int i = 0; i < 3; i++) {
        if (position[i] < tankMin[i] || position[i] > tankMax[i]) {
            changeDir = true;
            break;
        }
    }
    
    // Random direction changes
    if (changeDir || (rand() % 1000) < 5) {
        changeDirection();
    }
    
    // Calculate rotation angle based on direction
    rotationAngle = atan2(direction.z, direction.x);
}

void Fish::Draw(Shader &shader) {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
    
    shader.setMat4("model", modelMatrix);
    model.Draw(shader);
}

void Fish::changeDirection() {
    // Random new direction
    direction = glm::normalize(glm::vec3(
        (rand() % 100) / 100.0f - 0.5f,
        (rand() % 100) / 100.0f - 0.5f,
        (rand() % 100) / 100.0f - 0.5f
    ));
    
    // Ensure fish stays mostly horizontal
    direction.y *= 0.3f;
    direction = glm::normalize(direction);
}